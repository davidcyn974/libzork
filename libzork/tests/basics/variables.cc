#include <filesystem>
#include <gtest/gtest.h>

#include "libzork/story/node.hh"
#include "libzork/story/story.hh"
#include "libzork/variables/action.hh"
#include "libzork/variables/condition.hh"
#include "tests/utils.hh"

namespace
{
    Store setup_store()
    {
        Store store;
        store.set_variable("foo", 42);
        store.set_variable("bar", 69);
        return store;
    }
} // namespace

TEST(Variables, condition_equal)
{
    Store store = setup_store();
    Condition cond(store, "foo", "equal", 42);

    EXPECT_TRUE(cond.apply());

    store.set_variable("foo", 11);
    EXPECT_FALSE(cond.apply());
}

TEST(Variables, condition_greater)
{
    Store store = setup_store();
    Condition cond(store, "foo", "greater", 42);

    EXPECT_FALSE(cond.apply()); // 42 > 42?

    store.set_variable("foo", 43);
    EXPECT_TRUE(cond.apply()); // 43 > 42?

    store.set_variable("foo", 11);
    EXPECT_FALSE(cond.apply()); // 11 > 42?
}

TEST(Variables, condition_greater_equal)
{
    Store store = setup_store();
    Condition cond(store, "foo", "greater_equal", 42);

    EXPECT_TRUE(cond.apply()); // 42 >= 42?

    store.set_variable("foo", 43);
    EXPECT_TRUE(cond.apply()); // 43 >= 42?

    store.set_variable("foo", 11);
    EXPECT_FALSE(cond.apply()); // 11 >= 42?
}

TEST(Variables, condition_lower)
{
    Store store = setup_store();
    Condition cond(store, "foo", "lower", 42);

    EXPECT_FALSE(cond.apply()); // 42 < 42?

    store.set_variable("foo", 43);
    EXPECT_FALSE(cond.apply()); // 43 < 42?

    store.set_variable("foo", 11);
    EXPECT_TRUE(cond.apply()); // 11 < 42?
}

TEST(Variables, condition_lower_equal)
{
    Store store = setup_store();
    Condition cond(store, "foo", "lower_equal", 42);

    EXPECT_TRUE(cond.apply()); // 42 <= 42?

    store.set_variable("foo", 43);
    EXPECT_FALSE(cond.apply()); // 43 <= 42?

    store.set_variable("foo", 11);
    EXPECT_TRUE(cond.apply()); // 11 <= 42?
}

TEST(Variables, action_assign)
{
    Store store = setup_store();
    Action action(store, "foo", "assign", 69);

    EXPECT_EQ(42, store.get_variable("foo"));

    action.apply();
    EXPECT_EQ(69, store.get_variable("foo"));
}

TEST(Variables, action_sub)
{
    Store store = setup_store();
    Action action(store, "foo", "sub", -27);

    action.apply();
    EXPECT_EQ(69, store.get_variable("foo"));
}

TEST(Variables, action_add)
{
    Store store = setup_store();
    Action action(store, "foo", "add", 27);

    action.apply();
    EXPECT_EQ(69, store.get_variable("foo"));
}

TEST(Variables, node_condition)
{
    Store store = setup_store();

    const auto script = assets_base / "scripts" / "hello.txt";
    story::Node hello("hello", script);
    story::Node world("world", script);

    hello.add_choice(&world, "text", { Condition(store, "foo", "equal", 42) });
    EXPECT_FALSE(hello.list_choices(true).empty());
    EXPECT_NE(nullptr, hello.make_choice(0));

    store.set_variable("foo", 69);
    EXPECT_TRUE(hello.list_choices(true).empty());
    EXPECT_EQ(nullptr, hello.make_choice(0));
}

TEST(Variables, node_action)
{
    Store store = setup_store();

    const auto script = assets_base / "scripts" / "hello.txt";
    story::Node hello("hello", script);
    story::Node world("world", script);

    hello.add_choice(&world, "text", {},
                     { Action(store, "foo", "assign", 69) });
    EXPECT_EQ(42, store.get_variable("foo"));

    EXPECT_NE(nullptr, hello.make_choice(0));
    EXPECT_EQ(69, store.get_variable("foo"));
}
