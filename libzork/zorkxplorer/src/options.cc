#include "options.hh"

#include <getopt.h>
#include <iostream>

namespace
{
    constexpr option options[] = {
        { "story", required_argument, nullptr, 's' },
        { "smart", required_argument, nullptr, 'm' },
        { "title", required_argument, nullptr, 'n' },
        { "html", required_argument, nullptr, 'h' },
        { "state-url", required_argument, nullptr, 't' },
        { "username", required_argument, nullptr, 'u' },
        { "story-url", required_argument, nullptr, 'o' },
        { "remote-runner", required_argument, nullptr, 'r' }
    };

    std::string usage(const std::string& name)
    {
        return "usage: " + name
            + " (--story <story.yml> | (--story-url <url> --title <name>))"
              " [--smart <synonyms.yml> | --html <directory/>]"
              " [--state-url <url> --username <name>]"
              " [--remote-runner <url>]";
    };
} // namespace

Config parse_options(int argc, char** argv)
{
    Config config;
    int opt;
    while ((opt = getopt_long(argc, argv, "s:m:h:t:u:o:n:r:", options, nullptr))
           != -1)
    {
        switch (opt)
        {
        case 's': // --story
            if (config.network_story.has_value())
                throw std::invalid_argument(
                    "incompatible options: `--story` and `--story-url`");
            config.story_path = optarg;
            break;
        case 'o': // --story-url
            if (!config.story_path.empty())
                throw std::invalid_argument(
                    "incompatible options: `--story` and `--story-url`");
            if (config.network_story.has_value())
                config.network_story->url = optarg;
            else
                config.network_story = { .url = optarg };
            break;
        case 'n': // --title
            if (config.network_story.has_value())
                config.network_story->title = optarg;
            else
                config.network_story = { .title = optarg };
            break;
        case 'm': // --smart
            if (config.story_type == StoryType::HTML)
                throw std::invalid_argument(
                    "incompatble options: `--smart` and `--html`");
            config.story_type = StoryType::Smart;
            config.story_arg = optarg;
            break;
        case 'h': // --html
            if (config.story_type == StoryType::Smart)
                throw std::invalid_argument(
                    "incompatible options: `--smart` and `--html`");
            config.story_type = StoryType::HTML;
            config.story_arg = optarg;
            break;
        case 't': // --state-url
            if (config.network_store.has_value())
                config.network_store->url = optarg;
            else
                config.network_store = { .url = optarg };
            break;
        case 'u': // --username
            if (config.network_store.has_value())
                config.network_store->username = optarg;
            else
                config.network_store = { .username = optarg };
            break;
        case 'r': // --remote-runner
            config.remote_runner_url = optarg;
            break;
        default:
            throw std::invalid_argument(usage(argv[0]));
        }
    };

    if (config.story_path.empty()
        && (!config.network_story.has_value()
            || config.network_story->url.empty())
        && config.remote_runner_url.empty())
        throw std::invalid_argument("either option '--story', '--story-url' or "
                                    "'--remote-runner' is mandatory");
    if (config.network_story.has_value()
        && (config.network_story->url.empty()
            || config.network_story->title.empty()))
        throw std::invalid_argument("options '--story-url' and '--title' "
                                    "can't be passed separately");
    if (config.network_store.has_value()
        && (config.network_store->url.empty()
            || config.network_store->username.empty()))
        throw std::invalid_argument("options '--state-url' and '--username' "
                                    "can't be passed separately");
    if (!config.network_story.has_value() && config.remote_runner_url.empty()
        && !fs::is_regular_file(config.story_path))
        throw std::invalid_argument(config.story_path.string()
                                    + " is not a file");
    if (!config.remote_runner_url.empty()
        && (config.story_type != StoryType::Smart
            || !config.network_store.has_value()))
        throw std::invalid_argument(
            "the option '--remote-runner' requires the options '--smart', "
            "'--state-url' and '--username'");

    return config;
}
