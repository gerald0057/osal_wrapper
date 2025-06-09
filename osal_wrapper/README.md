# OSAL Wrapper

This project provides an OSAL (Operating System Abstraction Layer) wrapper that can be utilized in other CMake projects. The purpose of this wrapper is to abstract the underlying operating system functionalities, making it easier to develop cross-platform applications.

## Project Structure

```
osal_wrapper
├── src
│   └── main.c        # Contains a simple "Hello World" program
├── CMakeLists.txt    # CMake configuration file
└── README.md         # Project documentation
```

## Building the Project

To build the project, follow these steps:

1. Ensure you have CMake installed on your system.
2. Open a terminal and navigate to the `osal_wrapper` directory.
3. Create a build directory:
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

## Running the Program

After building the project, you can run the executable generated in the `build` directory:

```
./main
```

This will output:

```
Hello, World!
```

## Usage in Other Projects

To use this OSAL wrapper in other CMake projects, you can include it as a subdirectory or link it as a library. Make sure to adjust your CMake configuration accordingly to include the necessary paths and link against the OSAL wrapper.