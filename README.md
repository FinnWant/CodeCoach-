# CodeCoach+

Scaffold for CodeCoach+ — minimal C++ prototype.

Build (Windows, MSVC):

```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

If `nlohmann_json` or `SQLite3` are not found by CMake, install them or configure your package manager (vcpkg recommended).

This scaffold creates a `codecoach` executable that initializes a local SQLite DB at `data/codecoach.db`.
