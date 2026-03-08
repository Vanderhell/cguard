cmake_minimum_required(VERSION 3.12)
project(cguard VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# cguard is header-only — interface library
# ---------------------------------------------------------------------------
add_library(cguard INTERFACE)
target_include_directories(cguard INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Compiler warnings for users of the library
target_compile_options(cguard INTERFACE
    $<$<C_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

# ---------------------------------------------------------------------------
# Examples (optional, build with -DCGUARD_BUILD_EXAMPLES=ON)
# ---------------------------------------------------------------------------
option(CGUARD_BUILD_EXAMPLES "Build example programs" OFF)

if(CGUARD_BUILD_EXAMPLES)
    add_executable(example_scope_guard examples/example_scope_guard.c)
    target_link_libraries(example_scope_guard PRIVATE cguard)

    add_executable(example_result examples/example_result.c)
    target_link_libraries(example_result PRIVATE cguard)
endif()

# ---------------------------------------------------------------------------
# Tests (optional, build with -DCGUARD_BUILD_TESTS=ON)
# ---------------------------------------------------------------------------
option(CGUARD_BUILD_TESTS "Build test suite" OFF)

if(CGUARD_BUILD_TESTS)
    enable_testing()

    add_executable(test_scope_guard tests/test_scope_guard.c)
    target_link_libraries(test_scope_guard PRIVATE cguard)
    add_test(NAME test_scope_guard COMMAND test_scope_guard)

    add_executable(test_result tests/test_result.c)
    target_link_libraries(test_result PRIVATE cguard)
    add_test(NAME test_result COMMAND test_result)
endif()

# ---------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS cguard EXPORT cguardTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cguard)

install(EXPORT cguardTargets
    FILE cguardTargets.cmake
    NAMESPACE cguard::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cguard
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cguardConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cguardConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cguard
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cguardConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cguardConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cguardConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cguard
)
