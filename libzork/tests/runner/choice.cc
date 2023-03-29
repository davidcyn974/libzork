#include "libzork/runner/choice.hh"

#include <gtest/gtest.h>

#include "tests/interactive.hh"

namespace libzork_tests
{
    using MockedChoiceRunner = MockedInteractiveRunner<::ChoiceRunner>;

    class ChoiceRunner : public InteractiveRunnerTest<MockedChoiceRunner>
    {
    protected:
        virtual void SetUp(std::string story)
        {
            this->runner_ = std::make_unique<MockedChoiceRunner>(
                std::make_unique<story::Story>(assets_base / "stories" / story
                                               / "story.yml"),
                this->is_, this->os_);
            this->runner_->reset();
        }
    };

    TEST_F(ChoiceRunner, print)
    {
        SetUp(static_story_);

        const auto start = "Hello, world!\n\n1. Explore the cave\n2. Go "
                           "in the forest\n\n";

        TEST_EQ(ReadStep(), start,
                "::ChoiceRunner::print_script is invalid, "
                "other tests may fail because of it.");
    }

    TEST_F(ChoiceRunner, simple_step)
    {
        SetUp(static_story_);

        const auto cave = "This is a dark cave\n\n\n";

        TestStep("1", cave);
    }

    TEST_F(ChoiceRunner, multiple_steps)
    {
        SetUp(long_story_);

        const auto start = "Hello, world!\n\n1. Explore the cave\n2. Go in the "
                           "forest\n3. Go back home\n\n";
        const auto forest =
            "This is a large forest\n\n1. Explore the cave\n2. Go back\n\n";
        const auto cave = "This is a dark cave\n\n1. Go out\n\n";
        const auto home = "Hello, world!\n\n\n";

        TestStep(start);
        TestStep("2", forest);
        TestStep("1", cave);
        TestStep("1", start);
        TestStep("1", cave);
        TestStep("1", start);
        TestStep("3", home);
    }

    TEST_F(ChoiceRunner, step_not_present)
    {
        SetUp(static_story_);

        try
        {
            Step("3"); // Not present
            FAIL() << "Expected exception, but nothing was thrown.";
        }
        catch (const std::runtime_error&)
        {}
    }

    TEST_F(ChoiceRunner, invalid_step)
    {
        SetUp(static_story_);

        try
        {
            // Quoi ?
            Step("feur"); // Invalid
            FAIL() << "Expected exception, but nothing was thrown.";
        }
        catch (const std::runtime_error& e)
        {}
    }

    TEST_F(ChoiceRunner, no_choice)
    {
        SetUp(static_story_);

        try
        {
            Step(""); // Invalid
            FAIL() << "Expected exception, but nothing was thrown.";
        }
        catch (const std::runtime_error& e)
        {}
    }

    TEST_F(ChoiceRunner, actions)
    {
        SetUp(dynamic_story_);

        const std::string text =
            "You are in a dark cave. You have a strange potion "
            "in your left pocket. In a room\n"
            "on your left are evil goblins that look pretty "
            "strong. You shouldn't fight them\n"
            "without enough HP.\n\n";

        const std::vector<std::string> choices = {
            "Take the potion\n",
            "Train\n",
            "Attack the goblins\n",
        };

        TestStep(text + "1. " + choices.at(0) + "2. " + choices.at(1) + "\n");
        Step("1"); // Take potion: +10hp -> cannot take another one anymore
        TestStep(text + "1. " + choices.at(1) + "\n");
        Step("1"); // Train: +1xp -> can attack the goblins
        TestStep(text + "1. " + choices.at(1) + "2. " + choices.at(2) + "\n");
        Step("1"); // Train: +1xp -> same
        TestStep(text + "1. " + choices.at(1) + "2. " + choices.at(2) + "\n");
        Step("2"); // Attack the goblins -> end of the game

        TestStep("You attack the goblins. This a tough fight and you lose 10 "
                 "HP. You are finally\n"
                 "victorious and rewarded with a potion of immortality.\n"
                 "\n"
                 "Congratulations! You just completed the game!\n\n"
                 "\n");
    }

    // RUN

    TEST_F(ChoiceRunner, run_simple)
    {
        SetUp(static_story_);
        TestRun({ "1" }, 1, 2);
    }

    TEST_F(ChoiceRunner, run_error)
    {
        SetUp(static_story_);
        TestRun({ "69", "1" }, 2, 2);
    }

    TEST_F(ChoiceRunner, run_dynamic)
    {
        SetUp(dynamic_story_);

        std::vector<std::string> lines;
        TestRun({ "1", "2" /* impossible */, "1", "1", "1", "1",
                  "3" /* missclick */, "2" /* end of the game */ },
                8, 7);
    }

    TEST_F(ChoiceRunner, run_long)
    {
        SetUp(long_story_);
        TestRun({ "2", "1", "1", "1", "1", "3" }, 6, 7);
    }
} // namespace libzork_tests
