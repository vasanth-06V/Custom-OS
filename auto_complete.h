#ifndef AUTO_COMPLETE_H
#define AUTO_COMPLETE_H

#include <string>
#include <vector>
#include <unordered_set>

class AutoComplete {
public:
    AutoComplete();
    ~AutoComplete();
    
    // Load available commands from PATH
    void loadCommands();
    
    // Get completions for a given prefix
    char** getCompletions(const char* text);
    
private:
    // Set of available commands
    std::unordered_set<std::string> commands;
    
    // Builtin commands
    std::vector<std::string> builtins = {
        "cd", "pwd", "help", "exit", "run", "schedule", "tasks"
    };
    
    // Helper function to get all files in a directory
    std::vector<std::string> getFilesInDirectory(const std::string& path);
    
    // Static function for readline
    static char* completion_generator(const char* text, int state);
    
    // Static pointer to current instance for callback
    static AutoComplete* currentInstance;
};

#endif // AUTO_COMPLETE_H