#include <istream>
#include <libzork/runner/smart.hh>

#include "tests/interactive.hh"
#include "tests/utils.hh"

namespace libzork_tests
{
    using MockedSmartRunner = MockedInteractiveRunner<::SmartRunner>;

    class SmartRunnerBonus : public InteractiveRunnerTest<MockedSmartRunner>
    {
    public:
        virtual void SetUp()
        {
            this->SetUp(empty_story_);
        }

        virtual void SetUp(std::string story)
        {
            this->SetUp(story, "yaka.yml");
        }

        virtual void SetUp(std::string story, std::string synonyms)
        {
            this->runner_ = std::make_unique<MockedSmartRunner>(
                std::make_unique<story::Story>(assets_base / "stories" / story
                                               / "story.yml"),
                assets_base / "synonyms" / synonyms, this->is_, this->os_);
            this->runner_->reset();
        }
    };

    TEST_F(SmartRunnerBonus, quit)
    {
        SetUp(static_story_);

        try
        {
            // Should throw an exception because of empty second entry
            Step("quit");
        }
        catch (std::runtime_error&)
        {
            FAIL() << "Fail: Invalid error";
        }
        catch (...)
        {}
    }

    TEST_F(SmartRunnerBonus, jump)
    {
        SetUp(static_story_);

        testing::internal::CaptureStdout();

        try
        {
            // Should throw an exception because of empty second entry
            Step("jump\n");
        }
        catch (...)
        {
            TEST_EQ(testing::internal::GetCapturedStdout().empty(), false,
                    "Invalid output");
        }
    }

    TEST_F(SmartRunnerBonus, shout)
    {
        SetUp(static_story_);

        testing::internal::CaptureStdout();

        try
        {
            // Should throw an exception because of empty second entry
            Step("shout\n");
        }
        catch (...)
        {
            TEST_EQ(testing::internal::GetCapturedStdout().starts_with(
                        "Aaaarrrrgggghhhh!\n"),
                    true, "Invalid output");
        }
    }

    TEST_F(SmartRunnerBonus, brief)
    {
        SetUp(static_story_);

        testing::internal::CaptureStdout();

        try
        {
            // Should throw an exception because of empty second entry
            Step("brief\n");
        }
        catch (...)
        {
            TEST_EQ(*this->runner_->print_script_counter_, 1,
                    "Invalid number of call to print_script");
        }

        ::testing::internal::GetCapturedStdout();
    }

} // namespace libzork_tests
