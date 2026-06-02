#pragma once

#include <string>
#include <optional>

enum class CliCommand{
    InsertStudent,
    DeleteStudent,
    SearchStudent,
    UpdateInfo,
};

class CliArgs{
    public:

    CliArgs(int Argc, char* argc[]);

    std::optional<CliCommand> cliCommand(){
        return cli_command;
    }

    std::string filePath(){
        return filename_;
    }

    void printUsage();
    

    private:

    void parse(int argc, char* argv[]);

    std::string index_;
    std::string filename_;
    std::string program;
    std::optional<CliCommand> cli_command;
    std::optional<std::string> account_;
};