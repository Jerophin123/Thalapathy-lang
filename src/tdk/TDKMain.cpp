#include "commands/TDKCommand.hpp"
#include "commands/VersionCommand.cpp"
#include "commands/DoctorCommand.cpp"
#include "commands/HomeCommand.cpp"
#include "commands/EnvCommand.cpp"
#include "commands/InfoCommand.cpp"
#include "commands/ListToolsCommand.cpp"
#include "commands/NewCommand.cpp"
#include "commands/InitCommand.cpp"
#include "commands/CheckCommand.cpp"
#include "commands/BuildCommand.cpp"
#include "commands/RunCommand.cpp"
#include "commands/CleanCommand.cpp"
#include "commands/InspectCommand.cpp"
#include "commands/PackageCommand.cpp"
#include "commands/NaduCommand.cpp"
#include <iostream>
#include <map>
#include <memory>

using namespace thalapathy;

static void printTdkHelp() {
    std::cout << "THALAPATHY Development Kit (TDK)\n\n"
              << "Usage:\n"
              << "  tdk <command> [options]\n\n"
              << "Commands:\n"
              << "  version       Print TDK version\n"
              << "  doctor        Verify environment health\n"
              << "  home          Display active TDK_HOME\n"
              << "  env           Display detailed environmental status\n"
              << "  info          Display general kit info\n"
              << "  list-tools    List TDK core binaries\n"
              << "  new <name>    Create a new project\n"
              << "  init          Initialize current folder as a project\n"
              << "  check         Run semantic verification on manifest entry\n"
              << "  build         Compile project entry to .vijay artifact\n"
              << "  run           Run project entry directly\n"
              << "  clean         Delete project build output safely\n"
              << "  inspect <pkg> Display compiled artifact metadata\n"
              << "  package       Generate .tvkpkg distribution archive\n"
              << "  nadu <a.b.c>  Create a Java-style package (dirs + stub .tvk)\n"
              << "  help          Display this help text\n\n"
              << "Options:\n"
              << "  --json        Format command output as structured JSON\n"
              << "  --force / -f  Force execution / overwrite in init\n";
}

int main(int argc, char* argv[]) {
    TDKContext ctx;
    std::string subcommand = "";
    std::vector<std::string> cmdArgs;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--json") {
            ctx.jsonOutput = true;
        } else if (arg == "--force" || arg == "-f") {
            ctx.force = true;
        } else if (arg.rfind("-", 0) == 0) {
            std::cerr << "error: unknown flag '" << arg << "'\n";
            return 1;
        } else {
            if (subcommand.empty()) {
                subcommand = arg;
            } else {
                cmdArgs.push_back(arg);
            }
        }
    }

    if (subcommand.empty() || subcommand == "help") {
        printTdkHelp();
        return (subcommand == "help") ? 0 : 1;
    }

    std::map<std::string, std::unique_ptr<TDKCommand>> commands;
    commands["version"] = std::make_unique<VersionCommand>();
    commands["doctor"] = std::make_unique<DoctorCommand>();
    commands["home"] = std::make_unique<HomeCommand>();
    commands["env"] = std::make_unique<EnvCommand>();
    commands["info"] = std::make_unique<InfoCommand>();
    commands["list-tools"] = std::make_unique<ListToolsCommand>();
    commands["new"] = std::make_unique<NewCommand>();
    commands["init"] = std::make_unique<InitCommand>();
    commands["check"] = std::make_unique<CheckCommand>();
    commands["build"] = std::make_unique<BuildCommand>();
    commands["run"] = std::make_unique<RunCommand>();
    commands["clean"] = std::make_unique<CleanCommand>();
    commands["inspect"] = std::make_unique<InspectCommand>();
    commands["package"] = std::make_unique<PackageCommand>();
    commands["nadu"] = std::make_unique<NaduCommand>();

    auto it = commands.find(subcommand);
    if (it == commands.end()) {
        std::cerr << "error: unknown command '" << subcommand << "'\n";
        printTdkHelp();
        return 1;
    }

    return it->second->execute(ctx, cmdArgs);
}
