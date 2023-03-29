#pragma once

#include <gtest/gtest.h>
#include <libzork/story/story.hh>
#include <memory>
#include <string>

#include "utils.hh"

namespace libzork_tests
{
    template <class RunnerType>
    class RunnerTest : public testing::Test
    {
    public:
        virtual void SetUp(std::string story) = 0;

    protected:
        void SetUp() override
        {}

        void TearDown() override
        {}

        std::unique_ptr<RunnerType> runner_;

        static constexpr auto empty_story_ = "empty";
        static constexpr auto static_story_ = "static";
        static constexpr auto dynamic_story_ = "dynamic";
        static constexpr auto long_story_ = "long";
        static constexpr auto ogre_story_ = "ogre";
    };
} // namespace libzork_tests
