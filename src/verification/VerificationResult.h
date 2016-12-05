/*
  GridTools Libraries

  Copyright (c) 2016, GridTools Consortium
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  For information: http://eth-cscs.github.io/gridtools/
*/
#pragma once

#include <gtest/gtest.h>
#include <string>
#include "../Common.h"

GT_VERIFICATION_NAMESPACE_BEGIN

/**
 * @brief Store the result of a verification test (returned by @ref Verification::verify())
 *
 * @ingroup DycoreUnittestVerificationLibrary
 */
class VerificationResult {
  public:
    VerificationResult() : passed_(true), msg_("") {}
    VerificationResult(const VerificationResult &) = default;
    VerificationResult(VerificationResult &&) = default;
    VerificationResult &operator=(const VerificationResult &) = default;

    /**
     * @brief Construct a result with given state and message
     *
     * @param passed    Inital state of the result
     * @param msg       Initial message
     *
     * @see VerificationResult::merge()
     */
    VerificationResult(bool passed = true, std::string msg = "\n") : passed_(passed), msg_(msg) {}

    /**
     * @brief Merge the result of a test with the current result
     *
     * If one of the tests failed, the merged result will be marked as failed as well.
     * The error message strings will be concatenated and reformatted.
     *
     * @param result  Result to merge
     */
    void merge(VerificationResult result) noexcept {
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

GT_VERIFICATION_NAMESPACE_END
