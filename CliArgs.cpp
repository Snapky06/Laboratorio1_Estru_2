#include "CliArgs.hpp"
#include <iostream>

CliArgs::CliArgs(int argc, char* argv[]) {
    parse(argc, argv);
}

std::optional<CliCommand> CliArgs::cliCommand() const {
    return cli_command;
}

std::string CliArgs::filePath() const {
    return filename_;
}

std::optional<std::string> CliArgs::account() const {
    return account_;
}

void CliArgs::printUsage() {
    std::cout << "\nUsage: " << program
              << "\n agregar <archivo_json> \n"
              << " eliminar <no_cuenta>\n"
              << " buscar <no_cuenta>\n"
              << program << " actualizar <archivo_json>\n";
}

void CliArgs::parse(int argc, char* argv[]) {
    program = argv[0];

    if (argc < 3) {
        return;
    }

    std::string command = argv[1];
    std::string value = argv[2];

    if (command == "agregar") {
        cli_command = CliCommand::InsertStudent;
        filename_ = value;
    } else if (command == "eliminar") {
        cli_command = CliCommand::DeleteStudent;
        account_ = value;
    } else if (command == "buscar") {
        cli_command = CliCommand::SearchStudent;
        account_ = value;
    } else if (command == "actualizar") {
        cli_command = CliCommand::UpdateInfo;
        filename_ = value;
    } else {
        std::cerr << "Unknown command: " << command << "\n";
    }
}