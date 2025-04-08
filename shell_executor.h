#ifndef SHELL_EXECUTOR_H
#define SHELL_EXECUTOR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

class ShellExecutor {
public:
    ShellExecutor();
    ~ShellExecutor();
    
    // Execute a command with arguments
    bool execute(const std::vector<std::string>& args);
    
private:
    // Map of built-in commands
    std::unordered_map<std::string, std::function<bool(const std::vector<std::string>&)>> builtins;
    
    // Initialize built-in commands
    void initBuiltins();
    
    // Built-in command implementations
    bool cd(const std::vector<std::string>& args);
    bool pwd(const std::vector<std::string>& args);
    bool help(const std::vector<std::string>& args);
    
    // Execute external command
    bool executeExternal(const std::vector<std::string>& args);
};

#endif // SHELL_EXECUTOR_H