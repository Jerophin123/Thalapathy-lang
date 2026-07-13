cmake_minimum_required(VERSION 3.20)
project(Thalapathy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable strict warning flags
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

include(FetchContent)

# Fetch Catch2 for unit tests
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

# Include directories
include_directories(include)

# Source files for the Thalapathy library
set(LIB_SOURCES
    src/lexer/Lexer.cpp
    src/parser/Parser.cpp
    src/diagnostics/Diagnostics.cpp
    src/personality/Personality.cpp
    src/personality/PersonalityMessages.cpp
    src/semantic/Resolver.cpp
    src/interpreter/Interpreter.cpp
    src/ir/IR.cpp
    src/cli/CLI.cpp
    src/module/ModuleResolver.cpp
    src/module/ModuleLoader.cpp
    src/module/ModuleGraph.cpp
    src/net/Net.cpp
    src/format/Formatter.cpp
    src/format/FormatterConfig.cpp
    src/project/ProjectLoader.cpp
    src/installation/TdkHome.cpp
    src/distribution/ReleaseMetadata.cpp
    src/api/Toolchain.cpp
)

# Header files listing (for IDEs)
set(LIB_HEADERS
    include/thalapathy/project/ProjectManifest.hpp
    include/thalapathy/format/Formatter.hpp
    include/thalapathy/format/FormatterConfig.hpp
    include/thalapathy/lexer/Token.hpp
    include/thalapathy/lexer/Lexer.hpp
    include/thalapathy/ast/AST.hpp
    include/thalapathy/parser/Parser.hpp
    include/thalapathy/diagnostics/Diagnostics.hpp
    include/thalapathy/personality/Personality.hpp
    include/thalapathy/semantic/Symbol.hpp
    include/thalapathy/semantic/Resolver.hpp
    include/thalapathy/types/Types.hpp
    include/thalapathy/runtime/Value.hpp
    include/thalapathy/runtime/Environment.hpp
    include/thalapathy/interpreter/Interpreter.hpp
    include/thalapathy/ir/IR.hpp
    include/thalapathy/cli/CLI.hpp
    include/thalapathy/installation/TdkHome.hpp
    include/thalapathy/distribution/ReleaseMetadata.hpp
    include/thalapathy/Toolchain.hpp
    include/thalapathy/version/Version.hpp
)

add_library(thalapathy_lib STATIC ${LIB_SOURCES} ${LIB_HEADERS})

# Networking backend (HTTP client/server) needs the socket library on Windows.
if(WIN32)
    target_link_libraries(thalapathy_lib PUBLIC ws2_32)
endif()

# Installable toolchain library for embedding
add_library(thalapathy_toolchain STATIC ${LIB_SOURCES} ${LIB_HEADERS})
set_target_properties(thalapathy_toolchain PROPERTIES OUTPUT_NAME thalapathy_toolchain EXPORT_NAME toolchain)
target_include_directories(thalapathy_toolchain PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
if(WIN32)
    target_link_libraries(thalapathy_toolchain PUBLIC ws2_32)
endif()

# Executable CLI
add_executable(thalapathy src/main.cpp)
target_link_libraries(thalapathy PRIVATE thalapathy_lib)

# Inspector and Formatter tools
add_executable(vijaydump src/tools/VijayDump.cpp)
target_link_libraries(vijaydump PRIVATE thalapathy_lib)

add_executable(thalafmt src/tools/ThalaFmt.cpp)
target_link_libraries(thalafmt PRIVATE thalapathy_lib)

# TDK Project manager tool
add_executable(tdk src/tdk/TDKMain.cpp)
target_link_libraries(tdk PRIVATE thalapathy_lib)

# Testing configuration
enable_testing()

add_executable(thalapathy_tests
    tests/lexer/LexerTests.cpp
    tests/parser/ParserTests.cpp
    tests/semantic/ResolverTests.cpp
    tests/interpreter/InterpreterTests.cpp
    tests/interpreter/FeaturesTests.cpp
    tests/runtime/RuntimeTests.cpp
    tests/diagnostics/DiagnosticsTests.cpp
    tests/personality/PersonalityTests.cpp
    tests/ir/IRTests.cpp
    tests/cli/CLITests.cpp
    tests/cli/SimpleRunTests.cpp
    tests/cli/CheckFlagTests.cpp
    tests/cli/BuildFlagTests.cpp
    tests/cli/EmitIRTests.cpp
    tests/cli/VersionFlagTests.cpp
    tests/cli/HelpFlagTests.cpp
    tests/cli/CLIConflictTests.cpp
    tests/cli/CLICompatibilityTests.cpp
    tests/tdk/TDKTests.cpp
    tests/tools/ThalaFmtTests.cpp
    tests/tools/VijayDumpTests.cpp
    tests/main.cpp
)
target_link_libraries(thalapathy_tests PRIVATE thalapathy_lib Catch2::Catch2WithMain)
add_test(NAME AllTests COMMAND thalapathy_tests)

# ============================================================
# Installation Rules — TDK Distribution Layout
# ============================================================

# 1. Executables -> bin/
install(TARGETS thalapathy tdk vijaydump thalafmt
        RUNTIME DESTINATION bin)

# 2. Installable toolchain library -> lib/
install(TARGETS thalapathy_toolchain
        EXPORT THALAPATHYTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)

# 3. Public headers only -> include/thalapathy/
install(FILES
        include/thalapathy/Toolchain.hpp
        include/thalapathy/version/Version.hpp
        include/thalapathy/diagnostics/Diagnostics.hpp
        include/thalapathy/lexer/Token.hpp
        DESTINATION include/thalapathy)

# 4. Configuration -> conf/
install(FILES conf/formatter.conf
        DESTINATION conf)

# 5. Standard library -> lib/std/
install(FILES
        lib/std/io.tvk
        lib/std/math.tvk
        lib/std/text.tvk
        lib/std/collections.tvk
        lib/std/time.tvk
        lib/std/json.tvk
        lib/std/http.tvk
        lib/std/web.tvk
        lib/std/random.tvk
        lib/std/os.tvk
        lib/std/db.tvk
        lib/std/regex.tvk
        lib/std/datetime.tvk
        lib/std/log.tvk
        DESTINATION lib/std)

# 6. VS Code extension -> tools/vscode/
install(DIRECTORY editors/vscode-thalapathy
        DESTINATION tools/vscode)

# 7. CMake integration -> tools/cmake/
install(EXPORT THALAPATHYTargets
        FILE THALAPATHYTargets.cmake
        NAMESPACE THALAPATHY::
        DESTINATION tools/cmake)

# Generate and install package config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/THALAPATHYConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/THALAPATHYConfig.cmake"
    INSTALL_DESTINATION tools/cmake
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/THALAPATHYConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/THALAPATHYConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/THALAPATHYConfigVersion.cmake"
        DESTINATION tools/cmake)

# 8. Documentation -> docs/
install(FILES
        docs/TDK.md
        docs/TDK_COMMANDS.md
        docs/TDK_INSTALLATION.md
        docs/LANGUAGE_SPEC.md
        docs/LANGUAGE_GUIDE.md
        docs/MODULE_SYSTEM.md
        docs/STANDARD_LIBRARY.md
        docs/VIJAY_BINARY_FORMAT.md
        DESTINATION docs)

# 9. Examples -> examples/
install(DIRECTORY examples/hello examples/oop examples/modules examples/stdlib
                  examples/web examples/packages
        DESTINATION examples)

# 10. Legal -> legal/
install(FILES LICENSE DESTINATION legal)
install(FILES legal/THIRD_PARTY_NOTICES.md DESTINATION legal)

# 11. Root metadata
install(FILES VERSION DESTINATION .)
install(FILES RELEASE.json DESTINATION .)
install(FILES README.md DESTINATION .)
