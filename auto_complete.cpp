#include "auto_complete.h"
#include <iostream>
#include <dirent.h>
#include <cstring>
#include <cstdlib>
#include <readline/readline.h>

// Initialize static member
AutoComplete* AutoComplete::currentInstance = nullptr;

AutoComplete::AutoComplete() {
    currentInstance = this;
}

AutoComplete::~AutoComplete() {
    if (currentInstance == this) {
        currentInstance = nullptr;
    }
}

void AutoComplete::loadCommands() {
    // Add built-in commands
    for (const auto& cmd : builtins) {
        commands.insert(cmd);
    }
    
    // Get PATH environment variable
    char* path = getenv("PATH");
    if (!path) {
        std::cerr << "Error: PATH environment variable not found." << std::endl;
        return;
    }
    
    // Split PATH by colon
    std::string pathStr(path);
    size_t pos = 0;
    std::string token;
    
    while ((pos = pathStr.find(':')) != std::string::npos) {
        token = pathStr.substr(0, pos);
        
        // Get executable files in this directory
        auto files = getFilesInDirectory(token);
        for (const auto& file : files) {
            commands.insert(file);
        }
        
        pathStr.erase(0, pos + 1);
    }
    
    // Don't forget the last directory
    auto files = getFilesInDirectory(pathStr);
    for (const auto& file : files) {
        commands.insert(file);
    }
}

std::vector<std::string> AutoComplete::getFilesInDirectory(const std::string& path) {
    std::vector<std::string> files;
    DIR* dir = opendir(path.c_str());
    
    if (dir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string filename(entry->d_name);
            
            // Skip . and ..
            if (filename != "." && filename != "..") {
                files.push_back(filename);
            }
        }
        closedir(dir);
    }
    
    return files;
}

char** AutoComplete::getCompletions(const char* text) {
    return rl_completion_matches(text, &AutoComplete::completion_generator);
}

char* AutoComplete::completion_generator(const char* text, int state) {
    if (!currentInstance) {
        return nullptr;
    }
    
    static std::vector<std::string> matches;
    static size_t match_index = 0;
    
    // If this is the first call for this completion, find all matches
    if (state == 0) {
        matches.clear();
        match_index = 0;
        
        std::string textStr(text);
        
        // Find matching commands
        for (const auto& cmd : currentInstance->commands) {
            if (cmd.find(textStr) == 0) {
                matches.push_back(cmd);
            }
        }
        
        // Check for file completion if we're not at the beginning of the line
        if (rl_point > 0) {
            // Get current working directory files
            auto files = currentInstance->getFilesInDirectory(".");
            for (const auto& file : files) {
                if (file.find(textStr) == 0) {
                    matches.push_back(file);
                }
            }
        }
    }
    
    // Return the next match
    if (match_index < matches.size()) {
        return strdup(matches[match_index++].c_str());
    } else {
        // No more matches
        return nullptr;
    }
}
