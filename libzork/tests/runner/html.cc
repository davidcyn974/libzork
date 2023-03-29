#include <cwchar>
#include <gtest/gtest.h>
#include <libxml++/parsers/domparser.h>
#include <libzork/runner/html.hh>

#include "tests/runner.hh"

#define ASSERT_NOT_NULL(_node)                                                 \
    do                                                                         \
    {                                                                          \
        if (_node == nullptr)                                                  \
        {                                                                      \
            this->is_valid = false;                                            \
            return;                                                            \
        }                                                                      \
    } while (0);

namespace libzork_tests
{
    class HTMLRunner : public RunnerTest<::HTMLRunner>
    {
    protected:
        struct HTML
        {
            static std::map<std::string, HTMLRunner::HTML*> parsed_;

            std::string script;
            // key: target
            // value: pair(text, target's file)
            std::map<std::string, std::pair<std::string, HTMLRunner::HTML>>
                choices;

            bool is_valid = true;

            HTML() = default;

            HTML(fs::path html)
                : HTMLRunner::HTML(xmlpp::DomParser(html.string()), html)
            {}

            HTML(xmlpp::DomParser dom, const fs::path& path)
            {
                if (parsed_.contains(path.filename()))
                {
                    std::cerr << "Error: Infinite recursion" << std::endl;
                    return;
                }

                parsed_.emplace(path.filename(), this);

                ASSERT_NOT_NULL(dom.get_document());

                // <html>
                const auto pNode = dom.get_document()->get_root_node();
                ASSERT_NOT_NULL(pNode);

                // <body>
                const auto pBody = pNode->get_first_child()->get_next_sibling();
                ASSERT_NOT_NULL(pBody);

                // <p>
                const auto pScript =
                    pBody->get_first_child()->get_next_sibling();
                ASSERT_NOT_NULL(pScript);

                this->script = dynamic_cast<const xmlpp::Element*>(pScript)
                                   ->get_child_text()
                                   ->get_content();

                // <ol>
                const auto pChoices =
                    pScript->get_next_sibling()->get_next_sibling();
                ASSERT_NOT_NULL(pChoices);

                for (const auto& choice : pChoices->get_children())
                {
                    if (choice->get_name() == "text")
                        continue;

                    // <li><a>
                    const auto pLink = dynamic_cast<const xmlpp::Element*>(
                        choice->get_first_child());
                    ASSERT_NOT_NULL(pLink);

                    const auto href = pLink->get_attribute("href");
                    ASSERT_NOT_NULL(href);

                    const auto choiceText =
                        dynamic_cast<const xmlpp::Element*>(pLink)
                            ->get_child_text()
                            ->get_content();

                    const fs::path story(href->get_value());
                    if (!fs::exists(story))
                    {
                        // Error message should be displayed later using
                        // assert_files_exist.
                        return;
                    }

                    if (parsed_.contains(story.filename()))
                    {
                        this->choices.emplace(
                            story.filename().replace_extension("").string(),
                            std::make_pair(choiceText,
                                           *parsed_.at(story.filename())));
                        continue;
                    }

                    const auto script =
                        std::make_pair(choiceText, HTMLRunner::HTML(story));

                    if (!script.second.is_valid)
                    {
                        this->is_valid = false;
                        return;
                    }

                    this->choices.emplace(
                        story.filename().replace_extension("").string(),
                        script);
                }
            }
        };

    public:
        HTMLRunner() = default;

        virtual void SetUp(std::string story) override
        {
            this->SetUp(story, story);
        }

        /**
         * @brief Called when setting up a test.
         *
         * Instantiate the runner and call its run method.
         * Parse the resulting HTML file and check its validity.
         *
         * @param story     The name of the story
         * @param start     The name of the first html file to run
         * @param out_dir   The output directory passed to the HTMLRunner
         *                  constructor
         */
        virtual void SetUp(std::string story, std::string start)
        {
            const auto yaml = assets_base / "stories" / story / "story.yml";
            const auto html = this->out_dir_ / (start + ".html");

            this->runner_ = std::make_unique<::HTMLRunner>(
                std::make_unique<story::Story>(yaml), this->out_dir_);

            // Generate the HTML file.
            this->runner_->run();

            try
            {
                if (!fs::exists(html))
                {
                    FAIL() << "Fail: At least one expected HTML file was not "
                              "created.";
                }

                dom_.parse_file(html.string());
            }
            catch (const xmlpp::exception& e)
            {
                FAIL() << "\nFail: Generated HTML file is invalid.";
            }

            got_ = HTML(html);

            ASSERT_TRUE(got_.is_valid)
                << "Fail: Generated HTML file does not follow the given "
                   "instructions.";
        }

        void TearDown() override
        {
            for (const auto& file : this->created_files_)
                fs::remove(out_dir_ / (file + ".html"));
            HTML::parsed_.clear();
        }

        void assert_files_exist(const std::list<std::string> filenames)
        {
            this->created_files_ = filenames;

            for (const std::string file : filenames)
            {
                const auto file_exists =
                    fs::exists(this->out_dir_ / (file + ".html"));
                ASSERT_TRUE(file_exists)
                    << "Fail: At least one expected HTML file was not "
                       "created.";
            }
        }

        void assert_choice(const std::string& choice,
                           const std::string& expected_text,
                           const int expected_size)
        {
            const auto& text = this->got_.choices.at(choice).first;
            const auto& choices = this->got_.choices.at(choice).second.choices;

            ASSERT_EQ(text, expected_text);
            ASSERT_EQ(choices.size(), expected_size);
        }

    protected:
        HTML got_;
        xmlpp::DomParser dom_;
        fs::path out_dir_ = fs::current_path();
        std::list<std::string> created_files_{};
    };

    std::map<std::string, HTMLRunner::HTML*> HTMLRunner::HTML::parsed_{};

    TEST_F(HTMLRunner, simple)
    {
        SetUp(this->static_story_, "welcome");

        this->assert_files_exist({ "welcome", "cave", "forest" });

        ASSERT_EQ(this->got_.script, "Hello, world!\n");

        this->assert_choice("cave", "Explore the cave", 0);
        this->assert_choice("forest", "Go in the forest", 0);
    }

    TEST_F(HTMLRunner, empty)
    {
        this->out_dir_ = ".";
        this->runner_ = std::make_unique<::HTMLRunner>(
            std::make_unique<story::Story>(assets_base / "stories"
                                           / empty_story_ / "story.yml"),
            this->out_dir_);

        // Generate the HTML file.
        this->runner_->run();

        for (const auto& file : fs::directory_iterator(this->out_dir_.string()))
        {
            if (file.path().extension() == ".html")
                FAIL() << "Fail: Unrequired HTML files were created.";
        }
    }

    TEST_F(HTMLRunner, long_story)
    {
        SetUp(this->long_story_, "welcome");

        this->assert_files_exist({ "welcome", "cave", "forest", "home" });

        ASSERT_EQ(this->got_.script, "Hello, world!\n");
        ASSERT_EQ(this->got_.choices.size(), 3);

        this->assert_choice("cave", "Explore the cave", 1);
        this->assert_choice("forest", "Go in the forest", 2);
        this->assert_choice("home", "Go back home", 0);
    }

    TEST_F(HTMLRunner, create_directory)
    {
        this->out_dir_ /= "new_dir";

        SetUp(this->static_story_, "welcome");

        const auto directory_exists = fs::is_directory(this->out_dir_);
        ASSERT_TRUE(directory_exists);

        this->assert_files_exist({ "welcome", "cave", "forest" });

        ASSERT_EQ(this->got_.script, "Hello, world!\n");
        ASSERT_EQ(this->got_.choices.size(), 2);

        this->assert_choice("cave", "Explore the cave", 0);
        this->assert_choice("forest", "Go in the forest", 0);
    }

} // namespace libzork_tests
