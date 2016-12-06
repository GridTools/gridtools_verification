#include "unittest_environment.h"

GT_VERIFICATION_NAMESPACE_BEGIN

unittest_environment *unittest_environment::instance_ = nullptr;

unittest_environment &unittest_environment::get_instance() {
    if (!instance_)
        throw verification_exception("unittest_environment is not properly setup!");

    return (*instance_);
}

void unittest_environment::TearDown() {
    print_skipped_tests();
    reference_serializer_.reset();
    error_serializer_.reset();
}

std::string unittest_environment::test_name() const noexcept {
    const ::testing::TestInfo *testInfo = ::testing::UnitTest::GetInstance()->current_test_info();

    if (!testInfo) {
        error::warning("unittest_environment::testName(): requested test name when not inside a test");
        return std::string();
    }

    return std::string(testInfo->test_case_name()) + "." + std::string(testInfo->name());
}

void unittest_environment::skip_test(std::string spname) {
    if (spname.empty())
        spname = test_name();
    skipped_.push_back(spname);
}

void unittest_environment::print_skipped_tests() const noexcept {
    if (!skipped_.empty()) {
        cprintf(color::YELLOW, "[  SKIPPED ]");
        std::printf(" %i test%s skipped, listed below:\n",
            static_cast< int >(skipped_.size()),
            (skipped_.size() == 1 ? "" : "s"));

        for (const auto &tests : skipped_) {
            cprintf(color::YELLOW, "[  SKIPPED ]");
            std::printf(" %s\n", tests.c_str());
        }
    }
}

GT_VERIFICATION_NAMESPACE_END
