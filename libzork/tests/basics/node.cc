#include "libzork/story/node.hh"

#include <filesystem>
#include <gtest/gtest.h>

#include "tests/utils.hh"

TEST(Node, get_name)
{
    const story::Node node("hello", assets_base / "scripts" / "hello.txt");
    TEST_EQ("hello", node.get_name(), "Invalid node name");
}

TEST(Node, get_text)
{
    const story::Node node("hello", assets_base / "scripts" / "hello.txt");
    TEST_EQ("Hello, world!\n", node.get_text(), "Invalid node text");
}

TEST(Node, add_and_list_choices)
{
    const auto script = assets_base / "scripts" / "hello.txt";
    story::Node lorem("lorem", script);
    const story::Node ipsum("ipsum", script);
    const story::Node dolor("dolor", script);
    const story::Node sit("sit", script);
    const story::Node amet("amet", script);

    const auto successors = { ipsum, dolor, sit, amet };
    for (std::size_t i = 0; const auto& next : successors)
        lorem.add_choice(&next, "text" + std::to_string(i++));

    const auto choices = lorem.list_choices(false);
    ASSERT_EQ(successors.size(), choices.size());
    for (std::size_t i = 0; i < successors.size(); ++i)
        TEST_EQ("text" + std::to_string(i), choices[i], "Invalid choice name");
}

TEST(Node, make_choice)
{
    const auto script = assets_base / "scripts" / "hello.txt";
    story::Node hello("hello", script);
    const story::Node world("world", script);

    hello.add_choice(&world, "text");
    const auto also_world = hello.make_choice(0);

    EXPECT_EQ(&world, also_world);
    TEST_EQ(world.get_name(), also_world->get_name(), "Invalid successor name");
    TEST_EQ(world.get_text(), also_world->get_text(), "Invalid successor text");
}

TEST(Node, make_choice_empty)
{
    const story::Node node("hello", assets_base / "scripts" / "hello.txt");
    EXPECT_EQ(nullptr, node.make_choice(0));
}

TEST(Node, make_choice_oob)
{
    const auto script = assets_base / "scripts" / "hello.txt";
    story::Node hello("hello", script);
    const story::Node world("world", script);
    hello.add_choice(&world, "text");

    EXPECT_NE(nullptr, hello.make_choice(0));
    EXPECT_EQ(nullptr, hello.make_choice(1));
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
