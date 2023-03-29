#pragma once

#include <filesystem>
#include <vector>

#define TEST_EQ(a, b, msg)                                                     \
    {                                                                          \
        const auto got = a;                                                    \
        const auto expected = b;                                               \
        ASSERT_TRUE(got == expected) << "Fail: " << msg << ".";                \
    }

namespace fs = std::filesystem;

const auto assets_base = fs::path("test-assets");
