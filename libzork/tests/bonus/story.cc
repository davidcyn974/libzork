#include "libzork/story/story.hh"

#include <gtest/gtest.h>

#include "tests/utils.hh"

namespace libzork_tests
{
    class StoryBonus : public ::testing::Test
    {
    public:
        void SetUp(fs::path path)
        {
            this->story_ = std::make_unique<story::Story>(path);

            ::testing::internal::CaptureStdout();
            this->story_->save(std::cout);
            const auto out = ::testing::internal::GetCapturedStdout();

            this->yaml_ = YAML::Load(out);
        }

    protected:
        std::unique_ptr<story::Story> story_;
        YAML::Node yaml_;
    };

    TEST_F(StoryBonus, save_empty)
    {
        SetUp(assets_base / "stories" / "empty" / "story.yml");

        try
        {
            const auto node = yaml_["active-node"].as<std::string>();
            const auto vars = yaml_["variables"].as<YAML::Node>();

            TEST_EQ(node, "null", "Invalid content: active-node");
            TEST_EQ(vars.size(), 0, "Invalid content: variables");
        }
        catch (YAML::Exception&)
        {
            FAIL() << "Fail: Invalid format";
        }
    }

    TEST_F(StoryBonus, save)
    {
        SetUp(assets_base / "stories" / "static" / "story.yml");

        try
        {
            const auto node = yaml_["active-node"].as<std::string>();
            const auto vars = yaml_["variables"].as<YAML::Node>();

            TEST_EQ(node, "welcome", "Invalid content: active-node");
            TEST_EQ(vars.size(), 0, "Invalid content: variables");
        }
        catch (YAML::Exception&)
        {
            FAIL() << "Fail: Invalid format";
        }
    }

    TEST_F(StoryBonus, save_variables)
    {
        SetUp(assets_base / "stories" / "dynamic" / "story.yml");

        try
        {
            const auto node = yaml_["active-node"].as<std::string>();
            const auto vars =
                yaml_["variables"].as<std::map<std::string, int>>();

            TEST_EQ(node, "cave", "Invalid content: active-node");
            TEST_EQ(vars.size(), 3, "Invalid content: variables");

            TEST_EQ(vars.contains("xp"), true, "Key not found: xp");
            TEST_EQ(vars.at("xp"), 0, "Invalid content: xp");

            TEST_EQ(vars.contains("health"), true, "Key not found: health");
            TEST_EQ(vars.at("health"), 10, "Invalid content: health");

            TEST_EQ(vars.contains("potion_taken"), true,
                    "Key not found: potion_taken");
            TEST_EQ(vars.at("potion_taken"), 0,
                    "Invalid content: potion_taken");
        }
        catch (YAML::Exception& e)
        {
            FAIL() << "Fail: Invalid format: " << e.what();
        }
    }

    TEST_F(StoryBonus, save_change_variables)
    {
        SetUp(assets_base / "stories" / "dynamic" / "story.yml");

        this->story_->get_current()->make_choice(0);

        ::testing::internal::CaptureStdout();
        this->story_->save(std::cout);
        this->yaml_ = YAML::Load(::testing::internal::GetCapturedStdout());

        try
        {
            const auto node = yaml_["active-node"].as<std::string>();
            const auto vars =
                yaml_["variables"].as<std::map<std::string, int>>();

            TEST_EQ(node, "cave", "Invalid content: active-node");
            TEST_EQ(vars.size(), 3, "Invalid content: variables");

            TEST_EQ(vars.contains("xp"), true, "Key not found: xp");
            TEST_EQ(vars.at("xp"), 0, "Invalid content: xp");

            TEST_EQ(vars.contains("health"), true, "Key not found: health");
            TEST_EQ(vars.at("health"), 30, "Invalid content: health");

            TEST_EQ(vars.contains("potion_taken"), true,
                    "Key not found: potion_taken");
            TEST_EQ(vars.at("potion_taken"), 1,
                    "Invalid content: potion_taken");
        }
        catch (YAML::Exception& e)
        {
            FAIL() << "Fail: Invalid format: " << e.what();
        }
    }

    TEST_F(StoryBonus, restore_empty)
    {
        SetUp(assets_base / "stories" / "empty" / "story.yml");

        std::stringstream stream;
        const auto lines = { "active-node: ~", "variables: {}" };

        for (const auto& line : lines)
            stream << line << "\n";

        this->story_->restore(stream);

        TEST_EQ(this->story_->get_current(), nullptr,
                "Invalid content: active-node");
        // Cannot test the content of the Store
    }

    TEST_F(StoryBonus, restore)
    {
        SetUp(assets_base / "stories" / "dynamic" / "story.yml");

        std::stringstream stream;
        const auto lines = {
            "active-node: cave", "variables:",        "  potion_taken: 1",
            "  xp: 1000",        "  health: 1000000",
        };

        for (const auto& line : lines)
            stream << line << "\n";

        this->story_->restore(stream);

        if (this->story_->get_current() == NULL)
            FAIL() << "Invalid content: active-node";

        TEST_EQ(this->story_->get_current()->get_name(), "cave",
                "Invalid content: active-node");

        const auto choices = this->story_->get_current()->list_choices(false);
        const auto available = this->story_->get_current()->list_choices(true);

        TEST_EQ(available[0], choices[1], "Invalid variable: health");
    }

} // namespace libzork_tests
