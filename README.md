# OSAL Wrapper

This project provides an OSAL (Operating System Abstraction Layer) wrapper that can be utilized in other CMake projects. The purpose of this wrapper is to abstract the underlying operating system functionalities, making it easier to develop cross-platform applications.

## Project Structure

```
osal_wrapper/
├── osal_wrapper/      # OSAL library source and header files
├── example/           # Example usage code
├── CMakeLists.txt     # Top-level CMake configuration file
└── README.md          # Project documentation
```

## Building and Installing the Project

To build and install the project, follow these steps:

1. Ensure you have CMake installed on your system.
2. Open a terminal and navigate to the project root directory.
3. Create a build directory and enter it:
   ```
   mkdir build
   cd build
   ```
4. Run CMake to configure the project:
   ```
   cmake ..
   ```
5. Build the project:
   ```
   make
   ```
6. Install the library and headers (may require sudo):
   ```
   sudo make install
   ```

After installation, header files will be in `/usr/local/include`, and the library and CMake package files will be in `/usr/local/lib` and `/usr/local/lib/cmake/osal_wrapper`.

## Usage in Other Projects (find_package)

After installation, you can use `find_package(osal_wrapper REQUIRED)` in your own CMake project:

```cmake
find_package(osal_wrapper REQUIRED)
add_executable(my_app main.c)
target_link_libraries(my_app PRIVATE osal_wrapper::osal_wrapper)
```

No need to manually set include paths; they are handled by the package.

## Example

See the `example/` directory for usage, or use the following minimal example:

```c
#include "osal_wrapper.h"

int main() {
    osal_printf("Hello, OSAL!\n");
    return 0;
}
```
