#include "libzork/story/story.hh"

#include <cstdlib>
#include <fstream>
#include <gtest/gtest.h>

#include "tests/utils.hh"

namespace
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

    void write_story(const fs::path& in_path, const fs::path& out_path)
    {
        auto cmd =
            "dot -Tplain " + in_path.string() + " > " + out_path.string();
        int ret_code = std::system(cmd.c_str());
        int status = WEXITSTATUS(ret_code);
        if (status)
            FAIL() << "An error occured in the testsuite (return code "
                   << status << "). Please create a ticket.";
    }
} // namespace

TEST(Story, current_node)
{
    const story::Story story(assets_base / "stories" / "static" / "story.yml");
    const auto node = story.get_current();

    ASSERT_NE(nullptr, node);
    EXPECT_EQ("welcome", node->get_name());
    EXPECT_EQ("Hello, world!\n", node->get_text());
}

TEST(Story, empty_story)
{
    const story::Story story(assets_base / "stories" / "empty" / "story.yml");

    ASSERT_EQ(nullptr, story.get_current());
}

TEST(Story, all_choices)
{
    const story::Story story(assets_base / "stories" / "static" / "story.yml");

    const auto welcome = story.get_current();
    ASSERT_EQ(2, welcome->list_choices(false).size());
    EXPECT_EQ("cave", welcome->make_choice(0)->get_name());
    EXPECT_EQ("forest", welcome->make_choice(1)->get_name());

    const auto cave = welcome->make_choice(0);
    EXPECT_TRUE(cave->list_choices(false).empty());

    const auto forest = welcome->make_choice(1);
    EXPECT_TRUE(forest->list_choices(false).empty());
}

TEST(Story, set_current)
{
    story::Story story(assets_base / "stories" / "static" / "story.yml");

    const auto cave = story.get_current()->make_choice(0);
    const auto forest = story.get_current()->make_choice(1);

    story.set_current(cave);
    EXPECT_EQ(cave, story.get_current());
    EXPECT_EQ("cave", story.get_current()->get_name());

    story.set_current(forest);
    EXPECT_EQ(forest, story.get_current());
    EXPECT_EQ("forest", story.get_current()->get_name());
}

TEST(Story, dot_format)
{
    const story::Story story(assets_base / "stories" / "static" / "story.yml");

    const auto dot_file = assets_base / "student.dot";
    const auto plain_file = assets_base / "student.plain";
    std::ofstream os(assets_base / "student.dot");
    os << story;
    std::flush(os);
    write_story(dot_file, plain_file);

    const auto expected_dot_file =
        assets_base / "stories" / "static" / "story.dot";
    const auto expected_plain_file =
        assets_base / "stories" / "static" / "story.plain";
    write_story(expected_dot_file, expected_plain_file);

    std::ifstream student_plain(plain_file);
    std::ifstream expected_plain(expected_plain_file);

    std::stringstream actual;
    std::stringstream expected;
    actual << student_plain.rdbuf();
    expected << expected_plain.rdbuf();

    EXPECT_EQ(expected.str(), actual.str());
}
