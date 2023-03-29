#pragma once

#include <filesystem>
#include <vector>

#include "libzork/story/node.hh"

namespace fs = std::filesystem;

namespace story
{
    class Story
    {
    public:
        explicit Story(const fs::path& path);

        Story(const Story& story) = delete;
        Story(Story&& story) = default;

        Story& operator=(const Story& story) = delete;
        Story& operator=(Story&& story) = default;

        const std::string& get_title() const;
        const Node* get_current() const;
        void set_current(const Node* node);
    private:
        std::vector<std::unique_ptr<Node>> nodes_;
        std::unique_ptr<Store> store_;
    };

    std::ostream& operator<<(std::ostream& os, const Story& story);
} // namespace story
