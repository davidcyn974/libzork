#pragma once

#include <string>

#include "libzork/store/store.hh"

class Action
{
public:
    enum class Type
    {
        assign,
        add,
        sub,
    };

    Action(Store& store, const std::string& variable, const std::string& action,
           int value);

    void apply() const;
};
