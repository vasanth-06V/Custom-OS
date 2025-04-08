#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "command_parser.h"
#include "shell_executor.h"
#include "auto_complete.h"
#include "script_runner.h"
#include "task_scheduler.h"

// Global variables for autocomplete functionality
AutoComplete autoCompleter;

// Custom completion function for readline
char** completion_function(const char* text, int start, int end) {
    rl_attempted_completion_over = 1;
    return autoCompleter.getCompletions(text);
}

int main() {
    // Initialize components
    CommandParser parser;
    ShellExecutor executor;
    ScriptRunner scriptRunner;
    TaskScheduler scheduler;
    
    // Setup autocomplete
    autoCompleter.loadCommands();
    rl_attempted_completion_function = completion_function;
    
    // Start scheduler in background
    scheduler.start();
    
    std::cout << "Custom Shell v1.0 (LFS-based)" << std::endl;
    
    char* input;
    bool running = true;
    
    while (running) {
        // Get current working directory for prompt
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        
        // Create prompt with username and directory
        std::string prompt = std::string(getenv("USER")) + "@custom-shell:" + cwd + "$ ";
        
        // Get input with readline (supports history and editing)
        input = readline(prompt.c_str());
        
        // Add input to history if not empty
        if (input && *input) {
            add_history(input);
        }
        
        if (!input) {
            // EOF (Ctrl+D) - exit shell
            std::cout << std::endl;
            running = false;
            continue;
        }
        
        std::string command(input);
        free(input);
        
        if (command.empty()) {
            continue;
        }
        
        // Handle built-in exit command
        if (command == "exit") {
            running = false;
            continue;
        }
        
        // Check if it's a script execution
        if (command.substr(0, 4) == "run ") {
            std::string scriptName = command.substr(4);
            scriptRunner.execute(scriptName);
            continue;
        }
        
        // Check if it's a scheduler command
        if (command.substr(0, 9) == "schedule ") {
            scheduler.addTask(command.substr(9));
            continue;
        }
        
        if (command == "tasks") {
            scheduler.listTasks();
            continue;
        }
        
        // Normal command execution
        std::vector<std::string> args = parser.parse(command);
        executor.execute(args);
    }
    
    // Clean up
    scheduler.stop();
    
    std::cout << "Exiting custom shell." << std::endl;
    return 0;
}
