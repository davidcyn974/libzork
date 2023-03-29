#include <gtest/gtest.h>
#include <libzork/store/store.hh>
#include <libzork/story/node.hh>

#include "tests/utils.hh"

namespace libzork_tests
{
    class StoreBonus : public ::testing::Test
    {
    protected:
        Store store_;
    };

    TEST_F(StoreBonus, undo)
    {
        std::vector<story::Node> nodes = {
            { "hello", assets_base / "scripts" / "hello.txt" },
            { "hello", assets_base / "scripts" / "hello.txt" },
            { "hello", assets_base / "scripts" / "hello.txt" },
            { "hello", assets_base / "scripts" / "hello.txt" },
            { "hello", assets_base / "scripts" / "hello.txt" }
        };

        for (const auto& node : nodes)
            this->store_.set_active_node(&node);

        TEST_EQ(this->store_.get_active_node(), &nodes[4], "Invalid node");
        TEST_EQ(this->store_.undo(), true, "Invalid return value");
        TEST_EQ(this->store_.get_active_node(), &nodes[3], "Invalid node");
        TEST_EQ(this->store_.undo(), true, "Invalid return value");
        TEST_EQ(this->store_.get_active_node(), &nodes[2], "Invalid node");
        TEST_EQ(this->store_.undo(), true, "Invalid return value");
        TEST_EQ(this->store_.get_active_node(), &nodes[1], "Invalid node");
        TEST_EQ(this->store_.undo(), true, "Invalid return value");
        TEST_EQ(this->store_.get_active_node(), &nodes[0], "Invalid node");
        TEST_EQ(this->store_.undo(), false, "Invalid return value");
        TEST_EQ(this->store_.get_active_node(), &nodes[0], "Invalid node");
    }

    TEST_F(StoreBonus, undo_empty)
    {
        TEST_EQ(this->store_.undo(), false, "Invalid return value");
        TEST_EQ(this->store_.get_active_node(), nullptr, "Invalid node");
    }

    TEST_F(StoreBonus, undo_one)
    {
        const story::Node node("hello", assets_base / "scripts" / "hello.txt");
        this->store_.set_active_node(&node);

        TEST_EQ(this->store_.undo(), false, "Invalid return value");
        TEST_EQ(this->store_.get_active_node(), &node, "Invalid node");
    }

    TEST_F(StoreBonus, redo)
    {
        std::vector<story::Node> nodes = {
            { "hello", assets_base / "scripts" / "hello.txt" },
            { "hello", assets_base / "scripts" / "hello.txt" },
        };

        for (const auto& node : nodes)
            this->store_.set_active_node(&node);

        TEST_EQ(this->store_.get_active_node(), &nodes[1], "Invalid node");
        TEST_EQ(this->store_.undo(), true, "Undo is invalid.");
        TEST_EQ(this->store_.get_active_node(), &nodes[0], "Invalid node");
        TEST_EQ(this->store_.redo(), true, "Invalid return type");
        TEST_EQ(this->store_.get_active_node(), &nodes[1], "Invalid node");
    }

    TEST_F(StoreBonus, redo_empty)
    {
        TEST_EQ(this->store_.redo(), false, "Invalid return type");
        TEST_EQ(this->store_.get_active_node(), nullptr, "Invalid node");
    }

    TEST_F(StoreBonus, redo_err)
    {
        std::vector<story::Node> nodes = {
            { "hello", assets_base / "scripts" / "hello.txt" },
            { "hello", assets_base / "scripts" / "hello.txt" },
        };

        for (const auto& node : nodes)
            this->store_.set_active_node(&node);

        TEST_EQ(this->store_.redo(), false, "Invalid return type");
        TEST_EQ(this->store_.get_active_node(), &nodes[1], "Invalid node");
    }

    TEST_F(StoreBonus, redo_err_2)
    {
        std::vector<story::Node> nodes = {
            { "hello", assets_base / "scripts" / "hello.txt" },
            { "hello", assets_base / "scripts" / "hello.txt" },
        };

        for (const auto& node : nodes)
            this->store_.set_active_node(&node);

        TEST_EQ(this->store_.undo(), true, "Invalid return type");
        TEST_EQ(this->store_.get_active_node(), &nodes[0], "Invalid node");
        TEST_EQ(this->store_.redo(), true, "Invalid return type");
        TEST_EQ(this->store_.get_active_node(), &nodes[1], "Invalid node");
        TEST_EQ(this->store_.redo(), false, "Invalid return type");
        TEST_EQ(this->store_.get_active_node(), &nodes[1], "Invalid node");
    }

    TEST_F(StoreBonus, undo_cancel)
    {
        std::vector<story::Node> nodes = {
            { "hello", assets_base / "scripts" / "hello.txt" },
            { "hello", assets_base / "scripts" / "hello.txt" },
            { "hello", assets_base / "scripts" / "hello.txt" },
            { "hello", assets_base / "scripts" / "hello.txt" },
            { "hello", assets_base / "scripts" / "hello.txt" }
        };

        story::Node new_node("hello", assets_base / "scripts" / "hello.txt");

        for (const auto& node : nodes)
            this->store_.set_active_node(&node);

        TEST_EQ(this->store_.get_active_node(), &nodes[4], "Invalid node");
        TEST_EQ(this->store_.undo(), true, "Invalid return value");
        TEST_EQ(this->store_.get_active_node(), &nodes[3], "Invalid node");
        TEST_EQ(this->store_.undo(), true, "Invalid return value");
        TEST_EQ(this->store_.get_active_node(), &nodes[2], "Invalid node");

        // Add the new node: should not be able to redo what's been done anymore
        this->store_.set_active_node(&new_node);
        TEST_EQ(this->store_.redo(), false, "Invalid return value");
        TEST_EQ(this->store_.get_active_node(), &new_node, "Invalid node");
    }
} // namespace libzork_tests
