#include "CliArgs.hpp"
#include <iostream>

CliArgs::CliArgs(int argc, char* argv[]){
    parse(argc, argv);
}

void CliArgs::printUsage(){
    std::cout << "Usage: " << program << 
    "\n--file <filename> [--insert-student | --delete-student | --search-student]\n"
    << "\n--insert-student : Inserts a student\n"
    << "\n--delete-student <account> : Deletes an especified student\n"
    << "\n--search-student <account> : Searches for the student records at the specificied account\n"
    << std::endl;
}