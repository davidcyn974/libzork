#pragma once

#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

enum class StoryType
{
    Choice,
    Smart,
    HTML,
};

struct NetworkStoreConfig
{
    std::string url = "";
    std::string username = "";
};

struct NetworkStoryConfig
{
    std::string url = "";
    std::string title = "";
};

struct Config
{
    std::filesystem::path story_path;
    StoryType story_type = StoryType::Choice;
    fs::path story_arg; /** Undefined if story_type is StoryType::BASIC */
    std::optional<NetworkStoryConfig> network_story;
    std::optional<NetworkStoreConfig> network_store;
    std::string remote_runner_url;
};

Config parse_options(int argc, char** argv);
