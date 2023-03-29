#pragma once

#include <istream>
#include <memory>

#include "tests/runner.hh"

namespace libzork_tests
{
    template <class InteractiveRunner>
    class MockedInteractiveRunner : public InteractiveRunner
    {
    public:
        using InteractiveRunner::InteractiveRunner;

        void reset()
        {
            *process_input_counter_ = 0;
            *print_script_counter_ = 0;
        }

        void process_input() override
        {
            (*process_input_counter_)++;
            InteractiveRunner::process_input();
        }

        void print_script() const override
        {
            (*print_script_counter_)++;
            InteractiveRunner::print_script();
        }

        std::unique_ptr<int> process_input_counter_ =
            std::unique_ptr<int>(new int(0));
        std::unique_ptr<int> print_script_counter_ =
            std::unique_ptr<int>(new int(0));
    };

    template <class Runner>
    class InteractiveRunnerTest : public RunnerTest<Runner>
    {
    protected:
        InteractiveRunnerTest() = default;

    public:
        std::string ReadStep() const
        {
            testing::internal::CaptureStdout();
            this->runner_->print_script();
            return testing::internal::GetCapturedStdout();
        }

        void TestRun(const std::list<std::string>& entries, const int input,
                     const int output)
        {
            std::vector<std::string> lines;
            ASSERT_NO_THROW(lines = Run(entries));

            TEST_EQ(*this->runner_->process_input_counter_, input,
                    "Invalid number of calls to process_input");

            TEST_EQ(*this->runner_->print_script_counter_, output,
                    "Invalid number of calls to print_script");

            TEST_EQ(count_input_start(lines), input, "Invalid output format");
        }

        void Step(std::string step)
        {
            this->is_.flush();
            this->is_ << step << "\n";
            this->runner_->process_input();
        }

        void TestStep(const std::string str)
        {
            TEST_EQ(ReadStep(), str,
                    "Unexpected output. Check process_input and print_script");
        }

        void TestStep(const std::string step, const std::string expected)
        {
            Step(step);
            TestStep(expected);
        }

        std::stringstream is_;
        std::ostream& os_ = std::cout;

        static int count_input_start(const std::vector<std::string>& lines)
        {
            int count = 0;

            for (const auto& line : lines)
            {
                count += line.starts_with("> ");
            }

            return count;
        }

        const std::vector<std::string>
        Run(const std::list<std::string>& entries)
        {
            // Input the multiple entries
            for (const auto& entry : entries)
                this->is_ << entry << "\n";

            // Run
            testing::internal::CaptureStdout();
            this->runner_->run();
            const auto out = testing::internal::GetCapturedStdout();

            // Split output into lines

            std::vector<std::string> lines;
            size_t pos_start = 0;
            size_t pos_end;

            while ((pos_end = out.find("\n", pos_start)) != std::string::npos)
            {
                lines.push_back(out.substr(pos_start, pos_end - pos_start));
                pos_start = pos_end + 1;
            }

            lines.push_back(out.substr(pos_start));

            return lines;
        }
    };

} // namespace libzork_tests
