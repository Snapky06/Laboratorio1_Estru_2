#include "CliArgs.hpp"
#include <iostream>

CliArgs::CliArgs(int argc, char* argv[]){
    parse(argc, argv);
}

void CliArgs::printUsage(){
    std::cout << "Usage: " << program << 
    "\n--file <filename> [--insert-student | --delete-student <account>| --search-student<account>|--update-student<account>]\n"
    << "\n--insert-student : Inserts a student\n"
    << "\n--delete-student <account> : Deletes an especified student\n"
    << "\n--search-student <account> : Searches for the student records at the specificied account\n"
    << "\n--update-student <account> : Updates the student information at the specificied account\n"
    << std::endl;
}

void CliArgs::parse(int argc, char* argv[]){
    program = argv[0];

    for(int i = 1; i < argc; i++){
        std::string arg = argv[i];

        if(arg == "--file"){

            if(i + 1 < argc){
                filename_ = argv[i + 1];
                i++;
            } else {
                std::cerr << "A filepath is required after the argument --file\n";
            }

        } else if(arg == "--insert-student"){

            if(cli_command.has_value()){
                std::cerr << "Only one command at a time is permitted\n";
                return;
            }

            cli_command = CliCommand::InsertStudent;

        } else if(arg == "--delete-student"){

            if(cli_command.has_value()){
                std::cerr << "Only one command at a time is permitted\n";
                return;
            }

            cli_command = CliCommand::DeleteStudent;

            if(i + 1 < argc){
                account_ = argv[i + 1];
                i++;
            } else {
                std::cerr << "An account number is required after --delete-student\n";
                return;
            }

        } else if(arg == "--search-student"){

            if(cli_command.has_value()){
                std::cerr << "Only one command at a time is permitted\n";
                return;
            }

            cli_command = CliCommand::SearchStudent;

            if(i + 1 < argc){
                account_ = argv[i + 1];
                i++;
            } else {
                std::cerr << "An account number is required after --search-student\n";
                return;
            }

        } else if(arg == "--update-student"){

            if(cli_command.has_value()){
                std::cerr << "Only one command at a time is permitted\n";
                return;
            }

            cli_command = CliCommand::UpdateInfo;

            if(i + 1 < argc){
                account_ = argv[i + 1];
                i++;
            } else {
                std::cerr << "An account number is required after --update-student\n";
                return;
            }

        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
        }
    }
}