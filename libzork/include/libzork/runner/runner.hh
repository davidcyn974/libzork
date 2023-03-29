#pragma once

#include "libzork/story/story.hh"

class Runner
{
public:
    Runner(std::unique_ptr<story::Story> story);

    virtual void run() = 0;
};
