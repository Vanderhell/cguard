cmake_minimum_required(VERSION 3.16)
project(cguard_fetchcontent_consumer C)

include(FetchContent)
FetchContent_Declare(cguard
    SOURCE_DIR "${CGUARD_SOURCE_DIR}"
)
FetchContent_MakeAvailable(cguard)

add_executable(cguard_fetchcontent_consumer main.c)
target_link_libraries(cguard_fetchcontent_consumer PRIVATE cguard)
