#include "script_runner.h"
#include "command_parser.h"
#include "shell_executor.h"
#include <iostream>
#include <fstream>
#include <sstream>

ScriptRunner::ScriptRunner() {}

ScriptRunner::~ScriptRunner() {}

bool ScriptRunner::execute(const std::string& scriptPath) {
    // Parse script into commands
    auto commands = parseScript(scriptPath);
    
    if (commands.empty()) {
        std::cerr << "Error: Failed to parse script or script is empty." << std::endl;
        return false;
    }
    
    // Execute each command
    CommandParser parser;
    ShellExecutor executor;
    bool success = true;
    
    for (const auto& cmd : commands) {
        // Skip empty lines and comments
        if (cmd.empty() || cmd[0] == '#') {
            continue;
        }
        
        // Parse and execute command
        auto args = parser.parse(cmd);
        if (!executor.execute(args)) {
            std::cerr << "Error executing command: " << cmd << std::endl;
            success = false;
            
            // Check if we should continue on error
            if (cmd.find("set -e") != std::string::npos) {
                break;
            }
        }
    }
    
    return success;
}

std::vector<std::string> ScriptRunner::parseScript(const std::string& scriptPath) {
    std::vector<std::string> commands;
    std::ifstream file(scriptPath);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open script file: " << scriptPath << std::endl;
        return commands;
    }
    
    std::string line;
    std::string multiLineCommand;
    bool inMultiLine = false;
    
    while (std::getline(file, line)) {
        // Trim whitespace
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) {
            // Empty line
            if (!inMultiLine) {
                commands.push_back("");
            } else {
                multiLineCommand += "\n";
            }
            continue;
        }
        
        size_t end = line.find_last_not_of(" \t");
        line = line.substr(start, end - start + 1);
        
        // Check for line continuation
        if (!line.empty() && line.back() == '\\') {
            if (!inMultiLine) {
                inMultiLine = true;
                multiLineCommand = line.substr(0, line.length() - 1);
            } else {
                multiLineCommand += line.substr(0, line.length() - 1);
            }
        } else {
            if (inMultiLine) {
                multiLineCommand += line;
                commands.push_back(multiLineCommand);
                inMultiLine = false;
                multiLineCommand.clear();
            } else {
                commands.push_back(line);
            }
        }
    }
    
    // Handle case where script ends with a multiline command
    if (inMultiLine) {
        commands.push_back(multiLineCommand);
    }
    
    return commands;
}
