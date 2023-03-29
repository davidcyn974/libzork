enable_testing()

find_package(GTest)

# Finds the yaml-cpp library to link.
# find_library(YAML-CPP NAMES yaml-cpp libyaml-cpp REQUIRED)

# Find lixml++
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBXMLXX IMPORTED_TARGET libxml++-2.6)

IF(GTest_FOUND AND LIBXMLXX_FOUND)

    add_executable(testsuite
        # Uncomment these lines to add the tests
        # ./basics/variables.cc
        # ./basics/node.cc
        # ./basics/store.cc
        # ./basics/story.cc
        # ./bonus/smart.cc
        # ./bonus/store.cc
        # ./bonus/story.cc
        # ./runner/choice.cc
        # ./runner/smart.cc

        # This test depends on external libraries:
        #   - libxmlxx (nix-shell -p libxmlxx # On the PIE)
        # ./runner/html.cc
    )

    target_link_libraries(testsuite
        GTest::gtest GTest::gtest_main
        libzork
        ${YAML-CPP}
        PkgConfig::LIBXMLXX
    )

    # Includes the directory as public to libzork.
    target_include_directories(testsuite PUBLIC "include/")

    # Set C++ Standard
    set_target_properties(testsuite PROPERTIES
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF
    )

    file(COPY ../test-assets DESTINATION ${CMAKE_BINARY_DIR})

    add_test(testsuite testsuite)

ENDIF()
