#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string>
#include <vector>

class CommandParser {
public:
    CommandParser();
    ~CommandParser();
    
    // Parse a command string into a vector of arguments
    std::vector<std::string> parse(const std::string& command);
    
private:
    // Helper method to check if a character is a space
    bool isSpace(char c);
    
    // Helper method to handle quoted strings
    std::string parseQuotedString(const std::string& command, size_t& pos, char quote);
};

#endif // COMMAND_PARSER_H