#ifndef SCRIPT_RUNNER_H
#define SCRIPT_RUNNER_H

#include <string>
#include <vector>

class ScriptRunner {
public:
    ScriptRunner();
    ~ScriptRunner();
    
    // Execute a script file
    bool execute(const std::string& scriptPath);
    
private:
    // Parse script file into commands
    std::vector<std::string> parseScript(const std::string& scriptPath);
};

#endif // SCRIPT_RUNNER_H