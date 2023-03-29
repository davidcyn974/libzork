#pragma once

#include <iostream>

#include "runner.hh"

class InteractiveRunner : public Runner
{
public:
    using Runner::Runner;

    InteractiveRunner(std::unique_ptr<story::Story> story,
                      std::istream& is = std::cin,
                      std::ostream& os = std::cout);

    void run() override;

    virtual void print_script() const;
    virtual void process_input() = 0;

protected:
    std::istream& is_;
    std::ostream& os_;
};
