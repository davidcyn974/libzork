#include <istream>
#include <libzork/runner/smart.hh>

#include "tests/interactive.hh"

namespace libzork_tests
{
    using MockedSmartRunner = MockedInteractiveRunner<::SmartRunner>;

    class SmartRunner : public InteractiveRunnerTest<MockedSmartRunner>
    {
    public:
        virtual void SetUp()
        {
            this->SetUp(empty_story_);
        }

        virtual void SetUp(std::string story)
        {
            this->SetUp(story, "yaka.yml");
        }

        virtual void SetUp(std::string story, std::string synonyms)
        {
            this->runner_ = std::make_unique<MockedSmartRunner>(
                std::make_unique<story::Story>(assets_base / "stories" / story
                                               / "story.yml"),
                assets_base / "synonyms" / synonyms, this->is_, this->os_);
            this->runner_->reset();
        }
    };

    // COUNT SYNONYMS TEST

    TEST_F(SmartRunner, count_no_synonym)
    {
        const std::string a = "This sentence contains no synonym";
        const std::string b = "You can verify it yourself";

        ASSERT_EQ(this->runner_->count_synonyms(a, b), 0);
    }

    TEST_F(SmartRunner, count_synonyms_simple)
    {
        const std::string a = "Tom mange une saucisse";
        const std::string b = "C'est vraiment très uwu";

        ASSERT_EQ(this->runner_->count_synonyms(a, b), 1);
    }

    TEST_F(SmartRunner, count_synonyms_duplicate)
    {
        const std::string a = "plouc voleur voleur";
        const std::string b = "nicolas fidel nigel";

        ASSERT_EQ(this->runner_->count_synonyms(a, b), 2);
    }

    TEST_F(SmartRunner, count_synonyms_symmetry)
    {
        const std::string a = "bonjour hello";
        const std::string b = "hello bonjour";

        ASSERT_EQ(this->runner_->count_synonyms(a, b), 1);
    }

    TEST_F(SmartRunner, count_synonyms_same_word)
    {
        const std::string a = "bebou est ce que tu me gnogni";
        const std::string b = "bebou est ce que tu me gnogni";

        ASSERT_EQ(this->runner_->count_synonyms(a, b), 1);
    }

    TEST_F(SmartRunner, count_synonyms_separators)
    {
        const std::string a = "Go sur epita.sussy; bonjour,monde!";
        const std::string b = "T'es un vrai:baka? hello-world'";

        ASSERT_EQ(this->runner_->count_synonyms(a, b), 3);
    }

    // PROCESS INPUT TESTS

    TEST_F(SmartRunner, process_simple)
    {
        SetUp(ogre_story_, "ogre.yml");

        // cavern == cave
        TestStep("Enter the cavern", "This is a dark cave\n\n");
    }

    /*

    NOT TESTED

    TEST_F(SmartRunner, ProcessSameSentence)
    {
        SetUp(ogre_story_, "ogre.yml");

        // The exact same phrase as the first option, but count_synonyms = 0.
        TestStep("Explore the forest", "This is a large forest\n\n");
    }

    */

    TEST_F(SmartRunner, process_empty)
    {
        SetUp(ogre_story_, "ogre.yml");
        ASSERT_THROW(Step(""), std::runtime_error);
    }

    // RUN

    TEST_F(SmartRunner, run_simple)
    {
        SetUp(long_story_, "ogre.yml");
        TestRun({ "return home" }, 1, 2);
    }

    TEST_F(SmartRunner, run_long)
    {
        SetUp(long_story_, "ogre.yml");
        TestRun({ "enter the cavern", "return", "explore the woods",
                  "explore the cavern", "return", "return home" },
                6, 6);
    }

    TEST_F(SmartRunner, run_error)
    {
        SetUp(ogre_story_, "ogre.yml");
        TestRun({ "", "enter the cave" }, 2, 2);
    }

    TEST_F(SmartRunner, run_dynamic)
    {
        SetUp(dynamic_story_, "ogre.yml");
        TestRun({ "drink potion", "drink potion" /* impossible */, "train",
                  "train", "train", "train", "trina" /* missclick */,
                  "assault ogre" },
                8, 7);
    }

} // namespace libzork_tests
