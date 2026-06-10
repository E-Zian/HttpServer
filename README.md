# HTTP Server

A lightweight HTTP server written in C++ using ASIO for networking.

## Dependencies

- [CMake](https://cmake.org/) 
- [vcpkg](https://github.com/microsoft/vcpkg)
- [ASIO](https://think-async.com/Asio/) — networking
- [nlohmann-json](https://github.com/nlohmann/json) — JSON serialization
- [fmt](https://github.com/fmtlib/fmt) — string formatting

## Prerequisites

### 1. Install vcpkg
```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat  # Windows
```

### 2. Set the `VCPKG_ROOT` environment variable
```bash
# Windows (System Environment Variables)
VCPKG_ROOT = C:\path\to\vcpkg
```

> The CMake preset uses `$env{VCPKG_ROOT}` to locate the vcpkg toolchain file. The build will fail if this variable is not set.

## Building

### 1. Clone the repository
```bash
git clone https://github.com/E-Zian/HttpServer.git
cd HttpServer
```

### 2. Install dependencies via vcpkg
```bash
vcpkg install
```
This reads the `vcpkg.json` manifest and installs the stated dependencies automatically.

