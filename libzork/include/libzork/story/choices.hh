//
// Created by user_ on 04/03/2023.
//

#pragma once
#include <string>
#include <iostream>
#include "node.hh"

class choices {
private:
    std::string text_;
    const story::Node* target_node_;

public:
    choices(const std::string& text, const story::Node * target);
    const std::string& get_text() const;
    const story::Node* get_target_node() const;
};
