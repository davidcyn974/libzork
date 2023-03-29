#include <iostream>

#include "libzork/runner/choice.hh"
#include "libzork/runner/html.hh"
#include "libzork/runner/network.hh"
#include "libzork/runner/smart.hh"
#include "options.hh"

// Uncomment as you implement more features
// #define CHOICE_RUNNER
// #define SMART_RUNNER
// #define HTML_RUNNER

// #define NETWORK_STORE
// #define NETWORK_STORY
// #define NETWORK_RUNNER

std::unique_ptr<story::Story> get_story(const Config& config)
{
#ifdef NETWORK_STORE
    if (config.network_store.has_value())
    {
#    ifdef NETWORK_STORY
        if (config.network_story.has_value())
            return std::make_unique<story::Story>(
                config.network_story->url, config.network_story->title,
                config.network_store->url, config.network_store->username);
#    endif
        return std::make_unique<story::Story>(config.story_path,
                                              config.network_store->url,
                                              config.network_store->username);
    }
#endif
#ifdef NETWORK_STORY
    if (config.network_story.has_value())
        return std::make_unique<story::Story>(config.network_story->url,
                                              config.network_story->title);
#endif
    return std::make_unique<story::Story>(config.story_path);
}

std::unique_ptr<Runner>
get_runner(const Config& config,
           [[maybe_unused]] std::unique_ptr<story::Story> story)
{
    switch (config.story_type)
    {
#ifdef CHOICE_RUNNER
    case StoryType::Choice:
        return std::make_unique<ChoiceRunner>(std::move(story));
#endif
#ifdef SMART_RUNNER
    case StoryType::Smart:
        return std::make_unique<SmartRunner>(std::move(story),
                                             config.story_arg);
#endif
#ifdef HTML_RUNNER
    case StoryType::HTML:
        return std::make_unique<HTMLRunner>(std::move(story), config.story_arg);
#endif
    default:
        return nullptr;
    }
}

int main(int argc, char** argv)
{
    Config config;
    try
    {
        config = parse_options(argc, argv);
    }
    catch (const std::invalid_argument& exc)
    {
        std::cerr << "invalid options: " << exc.what() << "\n";
        return 1;
    }

    std::unique_ptr<Runner> runner;

#ifdef NETWORK_RUNNER
    if (!config.remote_runner_url.empty())
    {
        runner = std::make_unique<NetworkRunner>(
            config.remote_runner_url, config.story_arg,
            config.network_store->url, config.network_store->username);
    }
    else
#endif
    {
        auto story = get_story(config);
        runner = get_runner(config, std::move(story));
    }

    try
    {
        runner->run();
    }
    catch (const std::exception&)
    {
        /*
         * This is generally considered a bad practice to catch
         * std::exception but in this case we want to exit silently if the
         * 'quit' command is used.
         */
    }

    return 0;
}
