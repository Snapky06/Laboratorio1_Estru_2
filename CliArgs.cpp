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
        << "  " << program << " --file <name> [--index <name>] add --student <json_name>\n"
        << "  " << program << " --file <name> [--index <name>] search <account>\n"
        << "  " << program << " --file <name> [--index <name>] delete <account>\n"
        << "  " << program << " --file <name> [--index <name>] update --student <json_name>\n\n";
}

void CliArgs::parse(int argc, char* argv[])
{
    program = argv[0];

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "--file")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "--file requires a name\n";
                return;
            }

            filename_ = argv[++i];
        }
        else if (arg == "--index")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "--index requires a name\n";
                return;
            }

            index_ = argv[++i];
        }
        else if (arg == "--student")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "--student requires a JSON name\n";
                return;
            }

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
        else if (arg == "search")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "search requires an account\n";
                cli_command = std::nullopt;
                return;
            }

            cli_command = CliCommand::SearchStudent;
            account_ = argv[++i];
        }
        else if (arg == "delete")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "delete requires an account\n";
                cli_command = std::nullopt;
                return;
            }

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
    }

    if (filename_.size() < 4 || filename_.substr(filename_.size() - 4) != ".dat")
    {
        filename_ += ".dat";
    }

    if (index_.size() < 4 || index_.substr(index_.size() - 4) != ".idx")
    {
        index_ += ".idx";
    }

    if (student_file_.has_value())
    {
        if (student_file_->size() < 5 || student_file_->substr(student_file_->size() - 5) != ".json")
        {
            *student_file_ += ".json";
        }
    }

    if (!cli_command.has_value())
    {
        return;
    }

    if ((*cli_command == CliCommand::InsertStudent || *cli_command == CliCommand::UpdateInfo)
        && !student_file_.has_value())
    {
        std::cerr << "--student is required for add and update\n";
        cli_command = std::nullopt;
    }
}