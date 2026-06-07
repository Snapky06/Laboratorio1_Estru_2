#include "CliArgs.hpp"
#include "StudentManager.hpp"

#include <exception>
#include <iostream>
#include <optional>
#include <string>

void printStudent(const Student& s)
{
    std::cout << "Account: " << std::string(s.account, sizeof(s.account)) << "\n";
    std::cout << "Name: " << s.name << "\n";
    std::cout << "Telephone: " << std::string(s.telephone, sizeof(s.telephone)) << "\n";
    std::cout << "Age: " << s.age << "\n";
    std::cout << "Date: " << std::string(s.date, sizeof(s.date)) << "\n";
}

int main(int argc, char* argv[])
{
    CliArgs args(argc, argv);

    if (!args.cliCommand().has_value())
    {
        args.printUsage();
        return 1;
    }

    StudentManager manager(args.filePath(), args.indexPath());

    if (!manager.open())
    {
        std::cerr << "Could not open files\n";
        return 1;
    }

    bool ok = false;

    try
    {
        if (*args.cliCommand() == CliCommand::InsertStudent)
        {
            std::string json = *args.studentFile();
            ok = manager.addRegister(json);

            if (!ok)
            {
                std::cerr << "Add failed\n";
            }
        }
        else if (*args.cliCommand() == CliCommand::DeleteStudent)
        {
            ok = manager.deleteStudent(*args.account());

            if (!ok)
            {
                std::cerr << "Delete failed\n";
            }
        }
        else if (*args.cliCommand() == CliCommand::SearchStudent)
        {
            std::string account = *args.account();
            std::optional<Student> student = manager.searchStudent(account);

            if (student.has_value())
            {
                printStudent(*student);
                ok = true;
            }
            else
            {
                std::cerr << "Search failed\n";
            }
        }
        else if (*args.cliCommand() == CliCommand::UpdateInfo)
        {
            std::string json = *args.studentFile();
            ok = manager.updateStudent(json);

            if (!ok)
            {
                std::cerr << "Update failed\n";
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Invalid input: " << e.what() << "\n";
        ok = false;
    }

    manager.close();

    if (!ok)
    {
        return 1;
    }

    return 0;
}