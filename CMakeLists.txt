cmake_minimum_required(VERSION 3.16)
project(cguard VERSION 2.0.0 LANGUAGES C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(_cguard_default_build_tests OFF)
set(_cguard_default_build_examples OFF)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(_cguard_default_build_tests ON)
    set(_cguard_default_build_examples ON)
endif()

option(CGUARD_BUILD_TESTS "Build test suite" ${_cguard_default_build_tests})
option(CGUARD_BUILD_EXAMPLES "Build example programs" ${_cguard_default_build_examples})
option(CGUARD_STRICT_WARNINGS "Enable strict warnings for local targets" ON)
option(CGUARD_SCOPE_GUARD_ENABLE "Build scope-guard support" ON)
option(CGUARD_SCOPE_GUARD_REQUIRE "Require scope-guard support" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(result INTERFACE)
target_include_directories(result INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_library(scope_guard INTERFACE)
target_include_directories(scope_guard INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_definitions(scope_guard INTERFACE
    CGUARD_SCOPE_GUARD_ENABLE=$<BOOL:${CGUARD_SCOPE_GUARD_ENABLE}>
    CGUARD_SCOPE_GUARD_REQUIRE=$<BOOL:${CGUARD_SCOPE_GUARD_REQUIRE}>
)
target_link_libraries(scope_guard INTERFACE result)

add_library(cguard INTERFACE)
target_link_libraries(cguard INTERFACE result scope_guard)
target_include_directories(cguard INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

function(cguard_set_warnings target)
    if (MSVC)
        target_compile_options(${target} PRIVATE /W4)
        if (CGUARD_STRICT_WARNINGS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        if (CGUARD_STRICT_WARNINGS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()

if (CGUARD_BUILD_EXAMPLES)
    add_executable(example_result examples/example_result.c)
    target_link_libraries(example_result PRIVATE cguard)
    cguard_set_warnings(example_result)

    add_executable(example_scope_guard examples/example_scope_guard.c)
    target_link_libraries(example_scope_guard PRIVATE cguard)
    cguard_set_warnings(example_scope_guard)
endif()

if (CGUARD_BUILD_TESTS)
    enable_testing()

    add_executable(test_result tests/test_result.c)
    target_link_libraries(test_result PRIVATE cguard)
    cguard_set_warnings(test_result)
    add_test(NAME test_result COMMAND test_result)

    add_executable(test_scope_guard tests/test_scope_guard.c)
    target_link_libraries(test_scope_guard PRIVATE cguard)
    cguard_set_warnings(test_scope_guard)
    add_test(NAME test_scope_guard COMMAND test_scope_guard)

    add_executable(test_multi_tu
        tests/test_multi_tu.c
        tests/multi_tu_impl.c
    )
    target_link_libraries(test_multi_tu PRIVATE cguard)
    target_include_directories(test_multi_tu PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests)
    cguard_set_warnings(test_multi_tu)
    add_test(NAME test_multi_tu COMMAND test_multi_tu)

    add_executable(test_result_only tests/test_result_only.c)
    target_link_libraries(test_result_only PRIVATE result)
    target_compile_definitions(test_result_only PRIVATE CGUARD_SCOPE_GUARD_ENABLE=0)
    cguard_set_warnings(test_result_only)
    add_test(NAME test_result_only COMMAND test_result_only)
endif()

install(TARGETS result scope_guard cguard EXPORT cguardTargets)
install(DIRECTORY include/cguard DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

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
