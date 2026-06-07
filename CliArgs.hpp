#pragma once

#include <optional>
#include <string>

enum class CliCommand {
    InsertStudent,
    DeleteStudent,
    SearchStudent,
    UpdateInfo,
};

class CliArgs {
public:
    CliArgs(int argc, char* argv[]);

    std::optional<CliCommand> cliCommand() const;
    std::string filePath() const;
    std::string indexPath() const;
    std::optional<std::string> account() const;
    std::optional<std::string> studentFile() const;

    void printUsage() const;

private:
    void parse(int argc, char* argv[]);

    std::string filename_;
    std::string index_;
    std::string program;
    std::optional<CliCommand> cli_command;
    std::optional<std::string> account_;
    std::optional<std::string> student_file_;
};