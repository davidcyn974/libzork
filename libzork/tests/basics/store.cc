#include "libzork/store/store.hh"

#include <gtest/gtest.h>

#include "libzork/story/node.hh"
#include "tests/utils.hh"

// Active node

TEST(Store, node_init)
{
    const Store store;
    EXPECT_EQ(nullptr, store.get_active_node());
}

TEST(Store, node_structure)
{
    Store store;
    const story::Node node("hello", assets_base / "scripts" / "hello.txt");
    store.set_active_node(&node);

    TEST_EQ("hello", store.get_active_node()->get_name(),
            "Invalid active node name");
    TEST_EQ("Hello, world!\n", store.get_active_node()->get_text(),
            "Invalid active node text");
}

TEST(Store, node_ref)
{
    Store store;
    story::Node node("hello", assets_base / "scripts" / "hello.txt");
    store.set_active_node(&node);

    EXPECT_EQ(&node, store.get_active_node());
}

// Variables

TEST(Store, variables_simple)
{
    Store store;
    store.set_variable("foo", 42);

    EXPECT_EQ(42, store.get_variable("foo"));
}

TEST(Store, variables_many)
{
    Store store;
    const auto variables = { "foo",   "bar",    "baz",    "qux",  "quux",
                             "corge", "grault", "garply", "waldo" };

    for (std::size_t i = 0; const auto variable : variables)
        store.set_variable(variable, i++);

    for (std::size_t i = 0; const auto variable : variables)
        EXPECT_EQ(i++, store.get_variable(variable));
}

TEST(Store, variables_exists)
{
    Store store;
    EXPECT_FALSE(store.has_variable("foo"));

    store.set_variable("foo", 42);
    EXPECT_TRUE(store.has_variable("foo"));
}

TEST(Store, variable_unbound)
{
    Store store;
    store.set_variable("foo", 42);

    EXPECT_THROW({ store.get_variable("bar"); }, std::runtime_error);
}
