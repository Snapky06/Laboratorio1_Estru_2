#include "CliArgs.hpp"

int main(int argc, char* argv[])
{
    CliArgs args(argc, argv);

    if (!args.cliCommand().has_value())
    {
        args.printUsage();
        return 1;
    }

    return 0;
}
