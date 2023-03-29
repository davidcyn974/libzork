# Set cmake version.
cmake_minimum_required(VERSION 3.21.2)

# Create the executable from the source files.
add_executable(zorkxplorer)

# Set compilation flags.
target_compile_options(zorkxplorer PRIVATE -Wall -Wextra -Werror -pedantic -g)

# Set C++ Standard
set_target_properties(zorkxplorer PROPERTIES
  CXX_STANDARD 20
  CXX_EXTENSIONS OFF)

# Link against libzork
target_link_libraries(zorkxplorer PRIVATE libzork)

# Add src directory, to read its CMakeLists.txt file.
subdirs(src)
