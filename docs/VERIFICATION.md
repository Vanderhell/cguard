# Verification

Commands run locally in this workspace:

- `git status --short`
- `git branch --show-current`
- `git log -1 --oneline`
- `git tag --list`
- `make clean` not available on this Windows host because `make` is not installed
- `make tests` not available on this Windows host because `make` is not installed
- `make run-tests` not available on this Windows host because `make` is not installed
- `make examples` not available on this Windows host because `make` is not installed
- `cmake -S . -B build-test -DCGUARD_BUILD_TESTS=ON -DCGUARD_BUILD_EXAMPLES=ON`
- `cmake --build build-test --parallel`
- `ctest --test-dir build-test -C Debug --output-on-failure`
- `cmake --install build-test --config Debug --prefix C:\Users\vande\Desktop\github\cguard\build-install-prefix`
- `cmake -S tests\consumers\install_find_package -B build-consumer-install -DCMAKE_PREFIX_PATH=C:\Users\vande\Desktop\github\cguard\build-install-prefix`
- `cmake --build build-consumer-install --parallel`
- `cmake -S tests\consumers\fetchcontent -B build-consumer-fetch -DCGUARD_SOURCE_DIR=C:/Users/vande/Desktop/github/cguard`
- `cmake --build build-consumer-fetch --parallel`
- `build-consumer-install\Debug\cguard_install_consumer.exe`
- `build-consumer-fetch\Debug\cguard_fetchcontent_consumer.exe`
- `git diff --check`
- `where.exe cl.exe`
- `where.exe clang-cl.exe`
- `where.exe link.exe`
- `where.exe lld-link.exe`
- `Test-Path 'C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat'`
- `& 'C:\Windows\System32\cmd.exe' /c '"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" && "C:\Program Files\CMake\bin\cmake.exe" -S . -B build-msvc-debug -G Ninja -DCGUARD_BUILD_TESTS=ON -DCGUARD_BUILD_EXAMPLES=ON -DCGUARD_STRICT_WARNINGS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=cl -DCMAKE_MAKE_PROGRAM="C:/Users/vande/AppData/Local/Microsoft/WinGet/Packages/Ninja-build.Ninja_Microsoft.Winget.Source_8wekyb3d8bbwe/ninja.exe" && "C:\Program Files\CMake\bin\cmake.exe" --build build-msvc-debug --parallel && "C:\Program Files\CMake\bin\ctest.exe" --test-dir build-msvc-debug -C Debug --output-on-failure'`
- `& 'C:\Windows\System32\cmd.exe' /c '"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" && "C:\Program Files\CMake\bin\cmake.exe" -S . -B build-msvc-release -G Ninja -DCGUARD_BUILD_TESTS=ON -DCGUARD_BUILD_EXAMPLES=ON -DCGUARD_STRICT_WARNINGS=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cl -DCMAKE_MAKE_PROGRAM="C:/Users/vande/AppData/Local/Microsoft/WinGet/Packages/Ninja-build.Ninja_Microsoft.Winget.Source_8wekyb3d8bbwe/ninja.exe" && "C:\Program Files\CMake\bin\cmake.exe" --build build-msvc-release --parallel && "C:\Program Files\CMake\bin\ctest.exe" --test-dir build-msvc-release -C Release --output-on-failure'`
- `& 'C:\Windows\System32\cmd.exe' /c '"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" && "C:\Program Files\CMake\bin\cmake.exe" -S . -B build-clangcl-debug -G Ninja -DCGUARD_BUILD_TESTS=ON -DCGUARD_BUILD_EXAMPLES=ON -DCGUARD_STRICT_WARNINGS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang-cl.exe" -DCMAKE_MAKE_PROGRAM="C:/Users/vande/AppData/Local/Microsoft/WinGet/Packages/Ninja-build.Ninja_Microsoft.Winget.Source_8wekyb3d8bbwe/ninja.exe" && "C:\Program Files\CMake\bin\cmake.exe" --build build-clangcl-debug --parallel && "C:\Program Files\CMake\bin\ctest.exe" --test-dir build-clangcl-debug -C Debug --output-on-failure'`
- `& 'C:\Windows\System32\cmd.exe' /c '"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" && "C:\Program Files\CMake\bin\cmake.exe" -S . -B build-clangcl-release -G Ninja -DCGUARD_BUILD_TESTS=ON -DCGUARD_BUILD_EXAMPLES=ON -DCGUARD_STRICT_WARNINGS=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang-cl.exe" -DCMAKE_MAKE_PROGRAM="C:/Users/vande/AppData/Local/Microsoft/WinGet/Packages/Ninja-build.Ninja_Microsoft.Winget.Source_8wekyb3d8bbwe/ninja.exe" && "C:\Program Files\CMake\bin\cmake.exe" --build build-clangcl-release --parallel && "C:\Program Files\CMake\bin\ctest.exe" --test-dir build-clangcl-release -C Release --output-on-failure'`

The Visual Studio generator requires `-C Debug` for `ctest`.

WSL verification evidence from this run:

- `make clean`
- `make tests`
- `make run-tests`
- `make examples`
- WSL `make` emitted a clock-skew warning; it was non-blocking because independent clean CMake/Ninja verification passed afterward.
- GCC 13.3.0 Debug: `ctest` passed `4/4`.
- GCC 13.3.0 Release: `ctest` passed `4/4`.
- Clang 18.1.3 Debug: `ctest` passed `4/4`.
- Clang 18.1.3 Release: `ctest` passed `4/4`.
- Clang ASan/UBSan Debug: `ctest` passed `4/4`.
- Valgrind 3.22.0 passed with `0` leaks and `0` errors for `test_result`, `test_scope_guard`, `test_multi_tu`, and `test_result_only`.
- Install `find_package` consumer built and executed.
- FetchContent consumer built and executed.
