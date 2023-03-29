#pragma once

#include "interactive.hh"

class SmartRunner : public InteractiveRunner
{
public:
    SmartRunner(std::unique_ptr<story::Story> story,
                const fs::path& synonyms_path, std::istream& is = std::cin,
                std::ostream& os = std::cout);

    void process_input() override;
    int count_synonyms(const std::string& left, const std::string& right) const;
};
