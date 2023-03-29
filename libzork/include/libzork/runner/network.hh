#pragma once

#include "runner.hh"

class NetworkRunner : public Runner
{
public:
    NetworkRunner(const std::string& story_url, const fs::path& synonyms,
                  const std::string& store_url, const std::string& username);

    void run() override;
};
