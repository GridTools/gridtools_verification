/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <gtest/gtest.h>
#include <string>
#include "../common.h"

namespace gt_verification {

    /**
     * @brief Store the result of a verification test (returned by @ref Verification::verify())
     *
     * @ingroup DycoreUnittestVerificationLibrary
     */
    class verification_result {
      public:
        verification_result() : passed_(true), msg_("") {}
        verification_result(const verification_result &) = default;
        verification_result(verification_result &&) = default;
        verification_result &operator=(const verification_result &) = default;

        /**
         * @brief Construct a result with given state and message
         *
         * @param passed    Inital state of the result
         * @param msg       Initial message
         *
         * @see VerificationResult::merge()
         */
        verification_result(bool passed = true, std::string msg = "\n") : passed_(passed), msg_(msg) {}

        /**
         * @brief Merge the result of a test with the current result
         *
         * If one of the tests failed, the merged result will be marked as failed as well.
         * The error message strings will be concatenated and reformatted.
         *
         * @param result  Result to merge
         */
        void merge(verification_result result) noexcept {
            passed_ &= result.passed();
            if (!result.msg().empty())
                msg_ += "    " + result.msg() + "\n";
        }

        /**
         * @brief Check if any test failed
         */
        bool passed() const noexcept { return passed_; }

        /**
         * @brief Get the error message of the test (empty if passed == false)
         */
        std::string msg() const noexcept { return msg_; }

        /**
         * @brief Converts to true iff passed() == true
         */
        operator bool() const noexcept { return passed(); }

        /**
         * @brief Convert to GTest AssertionResult
         */
        testing::AssertionResult toAssertionResult() const noexcept {
            if (passed())
                return testing::AssertionSuccess();
            else
                return testing::AssertionFailure() << msg_.c_str();
        }

      private:
        bool passed_;
        std::string msg_;
    };
}
