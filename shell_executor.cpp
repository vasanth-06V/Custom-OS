#include "shell_executor.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <cerrno>

ShellExecutor::ShellExecutor() {
    initBuiltins();
}

ShellExecutor::~ShellExecutor() {}

void ShellExecutor::initBuiltins() {
    builtins["cd"] = [this](const std::vector<std::string>& args) { return this->cd(args); };
    builtins["pwd"] = [this](const std::vector<std::string>& args) { return this->pwd(args); };
    builtins["help"] = [this](const std::vector<std::string>& args) { return this->help(args); };
}

bool ShellExecutor::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        return true;
    }
    
    // Check if it's a built-in command
    auto it = builtins.find(args[0]);
    if (it != builtins.end()) {
        return it->second(args);
    }
    
    // Execute external command
    return executeExternal(args);
}

bool ShellExecutor::cd(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        // If no argument is provided, change to HOME directory
        chdir(getenv("HOME"));
        return true;
    }
    
    if (chdir(args[1].c_str()) != 0) {
        std::cerr << "cd: " << args[1] << ": " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

bool ShellExecutor::pwd(const std::vector<std::string>& args) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::cout << cwd << std::endl;
        return true;
    } else {
        std::cerr << "pwd: " << strerror(errno) << std::endl;
        return false;
    }
}

bool ShellExecutor::help(const std::vector<std::string>& args) {
    std::cout << "Custom Shell Help\n"
              << "Built-in commands:\n"
              << "  cd [dir]  - Change directory\n"
              << "  pwd       - Print working directory\n"
              << "  help      - Display this help\n"
              << "  exit      - Exit the shell\n"
              << "  run [script] - Execute a shell script\n"
              << "  schedule [time] [command] - Schedule a command to run\n"
              << "  tasks     - List scheduled tasks\n";
    return true;
}

bool ShellExecutor::executeExternal(const std::vector<std::string>& args) {
    pid_t pid = fork();
    
    if (pid == -1) {
        std::cerr << "Fork failed: " << strerror(errno) << std::endl;
        return false;
    } else if (pid == 0) {
        // Child process
        
        // Convert args to char* array for execvp
        char** c_args = new char*[args.size() + 1];
        for (size_t i = 0; i < args.size(); i++) {
            c_args[i] = strdup(args[i].c_str());
        }
        c_args[args.size()] = nullptr;
        
        // Execute the command
        execvp(c_args[0], c_args);
        
        // If execvp returns, it means there was an error
        std::cerr << args[0] << ": " << strerror(errno) << std::endl;
        
        // Clean up
        for (size_t i = 0; i < args.size(); i++) {
            free(c_args[i]);
        }
        delete[] c_args;
        
        // Exit child process
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
}
