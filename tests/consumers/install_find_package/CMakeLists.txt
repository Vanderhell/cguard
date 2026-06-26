cmake_minimum_required(VERSION 3.16)
project(cguard_install_consumer C)

find_package(cguard CONFIG REQUIRED)

add_executable(cguard_install_consumer main.c)
target_link_libraries(cguard_install_consumer PRIVATE cguard::cguard)
