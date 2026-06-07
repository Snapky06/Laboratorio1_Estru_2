#include "CliArgs.hpp"
#include "StudentManager.hpp"

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

    if (*args.cliCommand() == CliCommand::InsertStudent)
    {
        std::string json = *args.studentFile();
        ok = manager.addRegister(json);
    }
    else if (*args.cliCommand() == CliCommand::DeleteStudent)
    {
        ok = manager.deleteStudent(*args.account());
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
    }
    else if (*args.cliCommand() == CliCommand::UpdateInfo)
    {
        std::string json = *args.studentFile();
        ok = manager.updateStudent(json);
    }

    manager.close();

    if (!ok)
    {
        std::cerr << "Command failed\n";
        return 1;
    }

    return 0;
}