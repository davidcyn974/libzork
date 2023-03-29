cmake_minimum_required(VERSION 3.21.2)

project(libzork)

add_library(libzork SHARED
                    src/node.cc
                    src/story.cc
                    src/store.cc)

set_target_properties(libzork PROPERTIES OUTPUT_NAME zork)
set_target_properties(libzork
        PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
)

target_compile_options(libzork PRIVATE -Wall -Wextra -Werror -pedantic)

target_include_directories(libzork PUBLIC "include/")

find_library(YAML-CPP NAMES libyaml-cpp yaml-cpp REQUIRED)
target_link_libraries(libzork PRIVATE ${YAML-CPP})

subdirs(src)

add_subdirectory(zorkxplorer)
add_subdirectory(tests)

