#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "libzork/variables/action.hh"
#include "libzork/variables/condition.hh"
#include "choices.hh"

namespace fs = std::filesystem;

namespace story
{
    class Node
    {
    private:
        std::string name_;
        std::string text_;
        std::vector<const choices*> choices_;
        
    public:
        Node(const std::string& name, const fs::path& script_path);

        const std::string& get_name() const;
        const std::string& get_text() const;

        const Node* make_choice(std::size_t index) const;
        std::vector<std::string>
        list_choices(bool check_conditions = true) const;
        void add_choice(const Node* other, const std::string& text,
                        const std::vector<Condition>& conditions = {},
                        const std::vector<Action>& operations = {});
    };
} // namespace story
