#pragma once

#include "interactive.hh"

class ChoiceRunner : public InteractiveRunner
{
public:
    using InteractiveRunner::InteractiveRunner;

    void print_script() const override;
    void process_input() override;
};
