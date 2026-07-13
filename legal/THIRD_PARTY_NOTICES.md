# Third-Party Notices

This document lists third-party software used by the THALAPATHY Development Kit (TDK).

## Development/Test Dependencies

The following dependencies are used only during development and testing. They are NOT distributed with the installed TDK runtime.

### Catch2

- **License**: BSL-1.0 (Boost Software License 1.0)
- **Version**: 3.4.0
- **Source**: https://github.com/catchorg/Catch2
- **Usage**: Unit testing framework used during development builds only.
- **Distribution Status**: NOT included in TDK distribution. Catch2 is linked only into test executables which are not installed.

## Runtime Dependencies

The installed TDK runtime is a self-contained native C++20 application.

No external runtime libraries are required beyond the system C++ standard library.

The THALAPATHY toolchain does not embed or redistribute any third-party code in its installed binaries beyond the standard C++ runtime provided by the compiler toolchain.
