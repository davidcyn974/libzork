//
// Created by user_ on 04/03/2023.
//

#include "../include/libzork/story/choices.hh"

choices::choices(const std::string &text, const story::Node *target)
    :
    text_(text) , target_node_(target)
{}

const std::string &choices::get_text() const {
    return text_;
}

const story::Node *choices::get_target_node() const {
    return target_node_;
}


