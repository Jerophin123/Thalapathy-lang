#include <catch2/catch_test_macros.hpp>
#include "../../src/tdk/commands/TDKCommand.hpp"
#include "../../src/tdk/commands/VersionCommand.cpp"
#include "../../src/tdk/commands/HomeCommand.cpp"
#include "../../src/tdk/commands/EnvCommand.cpp"
#include "../../src/tdk/commands/InfoCommand.cpp"
#include "../../src/tdk/commands/ListToolsCommand.cpp"
#include "../../src/tdk/commands/DoctorCommand.cpp"
#include "../../src/tdk/commands/NewCommand.cpp"
#include "../../src/tdk/commands/InitCommand.cpp"
#include "../../src/tdk/commands/CheckCommand.cpp"
#include "../../src/tdk/commands/BuildCommand.cpp"
#include "../../src/tdk/commands/RunCommand.cpp"
#include "../../src/tdk/commands/CleanCommand.cpp"
#include "../../src/tdk/commands/InspectCommand.cpp"
#include "../../src/tdk/commands/PackageCommand.cpp"
#include "thalapathy/project/ProjectManifest.hpp"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using namespace thalapathy;

TEST_CASE("TDK Version Subcommand", "[tdk]") {
    TDKContext ctx;
    ctx.jsonOutput = false;
    VersionCommand cmd;
    int res = cmd.execute(ctx, {});
    CHECK(res == 0);

    ctx.jsonOutput = true;
    res = cmd.execute(ctx, {});
    CHECK(res == 0);
}

TEST_CASE("TDK Home Subcommand", "[tdk]") {
    TDKContext ctx;
    HomeCommand cmd;
    int res = cmd.execute(ctx, {});
    CHECK(res == 0);
}

TEST_CASE("TDK Env Subcommand", "[tdk]") {
    TDKContext ctx;
    EnvCommand cmd;
    int res = cmd.execute(ctx, {});
    CHECK(res == 0);
}

TEST_CASE("TDK Info Subcommand", "[tdk]") {
    TDKContext ctx;
    InfoCommand cmd;
    int res = cmd.execute(ctx, {});
    CHECK(res == 0);
}

TEST_CASE("TDK ListTools Subcommand", "[tdk]") {
    TDKContext ctx;
    ListToolsCommand cmd;
    int res = cmd.execute(ctx, {});
    CHECK(res == 0);
}

TEST_CASE("TDK Doctor Subcommand", "[tdk]") {
    TDKContext ctx;
    DoctorCommand cmd;
    int res = cmd.execute(ctx, {});
    // Should run successfully even if warnings (exit code 0 or 1)
    CHECK(res >= 0);
}

TEST_CASE("TDK Project Creation and Initialization", "[tdk]") {
    TDKContext ctx;
    ctx.jsonOutput = false;

    // Test NewCommand
    std::string testProjName = "test_created_proj";
    if (fs::exists(testProjName)) {
        fs::remove_all(testProjName);
    }

    NewCommand newCmd;
    int resNew = newCmd.execute(ctx, {testProjName});
    CHECK(resNew == 0);
    CHECK(fs::exists(testProjName));
    CHECK(fs::exists(fs::path(testProjName) / "thalapathy.toml"));
    CHECK(fs::exists(fs::path(testProjName) / "src" / "main.tvk"));

    // Save current working dir to restore later
    fs::path origPath = fs::current_path();
    fs::current_path(fs::path(testProjName));

    // Test InitCommand (should fail without force)
    InitCommand initCmd;
    int resInit = initCmd.execute(ctx, {});
    CHECK(resInit != 0);

    // Test InitCommand with force
    ctx.force = true;
    int resInitForce = initCmd.execute(ctx, {});
    CHECK(resInitForce == 0);

    // Restore path and cleanup
    fs::current_path(origPath);
    fs::remove_all(testProjName);
}

TEST_CASE("TDK Manifest Validation and Safe Path traversal", "[tdk]") {
    ProjectManifest manifest;
    std::string err;

    // Test valid manifest
    std::ofstream out("valid.toml");
    out << "[project]\n"
        << "name = \"valid\"\n"
        << "version = \"1.0.0\"\n"
        << "entry = \"src/main.tvk\"\n";
    out.close();

    bool ok = ProjectLoader::load("valid.toml", manifest, err);
    CHECK(ok);
    std::filesystem::remove("valid.toml");

    // Test path traversal rejection
    std::ofstream outBad("bad.toml");
    outBad << "[project]\n"
           << "name = \"bad\"\n"
           << "version = \"1.0.0\"\n"
           << "entry = \"../../etc/passwd.tvk\"\n";
    outBad.close();

    bool okBad = ProjectLoader::load("bad.toml", manifest, err);
    CHECK_FALSE(okBad);
    CHECK(err.find("path traversal") != std::string::npos);
    std::filesystem::remove("bad.toml");
}

TEST_CASE("TDK Safe Clean Subcommand", "[tdk]") {
    TDKContext ctx;

    // 1. Create a dummy project manifest
    std::ofstream out("thalapathy.toml");
    out << "[project]\n"
        << "name = \"safe_clean_test\"\n"
        << "version = \"1.0.0\"\n"
        << "entry = \"src/main.tvk\"\n"
        << "[build]\n"
        << "output = \"test_build_dir\"\n"
        << "artifact = \"safe_clean_test.vijay\"\n";
    out.close();

    fs::create_directories("test_build_dir");
    std::ofstream art("safe_clean_test.vijay");
    art << "dummy";
    art.close();

    // 2. Perform safe clean
    CleanCommand cleanCmd;
    int resClean = cleanCmd.execute(ctx, {});
    CHECK(resClean == 0);
    CHECK_FALSE(fs::exists("test_build_dir"));
    CHECK_FALSE(fs::exists("safe_clean_test.vijay"));

    // 3. Test clean safety against root directory deletion
    std::ofstream outUnsafe("thalapathy.toml");
    outUnsafe << "[project]\n"
              << "name = \"unsafe_clean\"\n"
              << "version = \"1.0.0\"\n"
              << "entry = \"src/main.tvk\"\n"
              << "[build]\n"
              << "output = \"/\"\n" // unsafe root output
              << "artifact = \"unsafe.vijay\"\n";
    outUnsafe.close();

    int resCleanUnsafe = cleanCmd.execute(ctx, {});
    CHECK(resCleanUnsafe != 0); // Must reject deletion

    std::filesystem::remove("thalapathy.toml");
}

TEST_CASE("TDK Distribution Layout Validation", "[tdk][distribution]") {
    const char* envHome = std::getenv("TDK_HOME");
    if (envHome && fs::exists(envHome)) {
        fs::path home(envHome);
        CHECK(fs::exists(home / "bin"));
        CHECK(fs::exists(home / "conf" / "formatter.conf"));
        CHECK(fs::exists(home / "lib" / "std" / "io.tvk"));
        CHECK(fs::exists(home / "lib" / "std" / "math.tvk"));
        CHECK(fs::exists(home / "lib" / "std" / "text.tvk"));
        CHECK(fs::exists(home / "lib" / "std" / "collections.tvk"));
        CHECK(fs::exists(home / "lib" / "std" / "time.tvk"));
        CHECK(fs::exists(home / "VERSION"));
        CHECK(fs::exists(home / "RELEASE.json"));
        CHECK(fs::exists(home / "README.md"));
        CHECK(fs::exists(home / "docs"));
        CHECK(fs::exists(home / "legal" / "LICENSE"));
        CHECK(fs::exists(home / "legal" / "THIRD_PARTY_NOTICES.md"));
    }
}
