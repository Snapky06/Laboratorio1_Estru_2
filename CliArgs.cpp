#include "CliArgs.hpp"

#include <iostream>

CliArgs::CliArgs(int argc, char* argv[])
{
    parse(argc, argv);
}

std::optional<CliCommand> CliArgs::cliCommand() const
{
    return cli_command;
}

std::string CliArgs::filePath() const
{
    return filename_;
}

std::string CliArgs::indexPath() const
{
    return index_;
}

std::optional<std::string> CliArgs::account() const
{
    return account_;
}

std::optional<std::string> CliArgs::studentFile() const
{
    return student_file_;
}

void CliArgs::printUsage() const
{
    std::cout
        << "Usage:\n"
        << "  " << program << " --file <filename.dat> [--index <indexname.idx>] add --student <json_file>\n"
        << "  " << program << " --file <filename.dat> [--index <indexname.idx>] delete <account>\n"
        << "  " << program << " --file <filename.dat> [--index <indexname.idx>] search <account>\n"
        << "  " << program << " --file <filename.dat> [--index <indexname.idx>] update --student <json_file>\n";
}

void CliArgs::parse(int argc, char* argv[])
{
    program = argv[0];

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "--file" && i + 1 < argc)
        {
            filename_ = argv[++i];
        }
        else if (arg == "--index" && i + 1 < argc)
        {
            index_ = argv[++i];
        }
        else if (arg == "--student" && i + 1 < argc)
        {
            student_file_ = argv[++i];
        }
        else if (arg == "add")
        {
            cli_command = CliCommand::InsertStudent;
        }
        else if (arg == "update")
        {
            cli_command = CliCommand::UpdateInfo;
        }
        else if (arg == "search" && i + 1 < argc)
        {
            cli_command = CliCommand::SearchStudent;
            account_ = argv[++i];
        }
        else if (arg == "delete" && i + 1 < argc)
        {
            cli_command = CliCommand::DeleteStudent;
            account_ = argv[++i];
        }
        else
        {
            std::cerr << "Invalid argument: " << arg << "\n";
            cli_command = std::nullopt;
            return;
        }
    }

    if (filename_.empty())
    {
        std::cerr << "--file is required\n";
        cli_command = std::nullopt;
        return;
    }

    if (index_.empty())
    {
        index_ = filename_;

        size_t dot = index_.find_last_of('.');

        if (dot != std::string::npos)
        {
            index_ = index_.substr(0, dot);
        }

        index_ += ".idx";
    }

    if (!cli_command.has_value())
    {
        return;
    }

    if ((*cli_command == CliCommand::InsertStudent || *cli_command == CliCommand::UpdateInfo)
        && !student_file_.has_value())
    {
        std::cerr << "--student is required for adding and updating\n";
        cli_command = std::nullopt;
    }
}