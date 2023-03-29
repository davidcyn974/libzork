#pragma once

#include <string>

#include "libzork/store/store.hh"

class Condition
{
public:
    enum class Type
    {
        equal,
        greater,
        lower,
        greater_equal,
        lower_equal,
    };

    Condition(const Store& store, const std::string& variable,
              const std::string& comparison, int value);

    bool apply() const;
};
