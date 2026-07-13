# THALAPATHY Memory Model

## Reference & Value Model
THALAPATHY 0.1.0 manages object, array, and map values dynamically via standard managed layouts in C++.
- Primitive values (integers, floats, bools, chars) use standard C++ stack copying inside the `std::variant`.
- Reference types (Arrays, Maps, Functions, Class instances) are allocated on the heap wrapped inside `std::shared_ptr`.
- Circular dependency cycles on object nodes are minimized through lexical scope teardown and lexical garbage collection.

## Memory Management Roadmap
Future versions will explore custom heap allocations and standard garbage collection strategies.
