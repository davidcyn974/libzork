//
// Created by user_ on 04/03/2023.
//

#include "libzork/story/node.hh"
#include "libzork/variables/condition.hh"

#include <fstream>


namespace fs = std::filesystem;
namespace story
{

    Node::Node(const std::string &name, const fs::path &script_path)
            :
            name_(name)
    {
        std::ifstream file_in(script_path);
        std::string token;
        while (std::getline(file_in, token))
        {
            text_ += token + "\n";
        }


    }

    const std::string &Node::get_name() const {
        return name_;
    }

    const std::string& Node::get_text() const
    {
        return text_;
    }

    const Node* Node::make_choice(std::size_t index) const
    {
        if (index >= choices_.size())
        {
            return nullptr;
        }
        return choices_[index]->get_target_node();
    }

    std::vector<std::string> Node::list_choices(bool check_conditions) const
    {
        std::vector<std::string> result;
        for (const auto& choice : choices_)
        {
            if (check_conditions) //|| choice.check_conditions())
            {
                result.push_back(choice->get_text());
            }
        }
        return result;
    }

    void Node::add_choice(const Node* other, const std::string& text,
                          (void) const std::vector<Condition>& conditions,
                          (void) const std::vector<Action>& actions)
    {
        choices_.push_back(choices(text,other));
    }



}
