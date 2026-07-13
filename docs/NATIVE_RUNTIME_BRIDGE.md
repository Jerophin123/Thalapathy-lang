# THALAPATHY Native Runtime Bridge

To support operating-system access without exposing unsafe FFI pointers or dynamic-link library loading, TDK establishes a secure **Native Runtime Bridge**.

---

## 1. Native Builtin Registrations
The runtime registers trusted C++ functions in the global environment environment. These functions are prefixed with `__native_` to distinguish them from standard program scopes:

- **`__native_read_line`**: Invokes `std::getline` from `std::cin` to retrieve interactive user inputs.
- **`__native_time_now`**: Invokes `std::chrono::system_clock` to fetch real high-precision system epoch timestamps in milliseconds.

---

## 2. Standard Library Boundary
The standard library modules (e.g., `io.tvk` and `time.tvk`) serve as the official language boundary. They wrap the internal `__native_` calls in safe, standard THALAPATHY functions (`vaasi()`, `now()`), preventing user applications from directly interacting with raw primitives.
