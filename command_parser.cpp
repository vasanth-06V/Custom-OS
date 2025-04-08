#include "command_parser.h"

CommandParser::CommandParser() {}

CommandParser::~CommandParser() {}

std::vector<std::string> CommandParser::parse(const std::string& command) {
    std::vector<std::string> args;
    std::string currentArg;
    bool inArg = false;
    
    for (size_t i = 0; i < command.length(); i++) {
        char c = command[i];
        
        // Handle quoted strings
        if (c == '"' || c == '\'') {
            if (inArg) {
                currentArg += parseQuotedString(command, i, c);
            } else {
                inArg = true;
                currentArg = parseQuotedString(command, i, c);
            }
            continue;
        }
        
        if (isSpace(c)) {
            if (inArg) {
                args.push_back(currentArg);
                currentArg.clear();
                inArg = false;
            }
        } else {
            if (!inArg) {
                inArg = true;
            }
            currentArg += c;
        }
    }
    
    // Don't forget the last argument
    if (inArg) {
        args.push_back(currentArg);
    }
    
    return args;
}

bool CommandParser::isSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

std::string CommandParser::parseQuotedString(const std::string& command, size_t& pos, char quote) {
    std::string result;
    pos++; // Skip the opening quote
    
    while (pos < command.length() && command[pos] != quote) {
        // Handle escape sequences
        if (command[pos] == '\\' && pos + 1 < command.length()) {
            pos++;
            result += command[pos];
        } else {
            result += command[pos];
        }
        pos++;
    }
    
    return result;
}
