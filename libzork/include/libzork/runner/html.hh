#pragma once

#include "runner.hh"

class HTMLRunner : public Runner
{
public:
    HTMLRunner(std::unique_ptr<story::Story> story, const fs::path& output_dir);

    void run() override;
};
