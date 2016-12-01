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

// TODO this test is bad because it needs to workaround the CommandLine singleton

//#include <verification/VerificationSpecification.h>
//#include <gmock/gmock.h>
//
// using namespace gt_verification;
//
///**
// * Test the VerificationSpecification without compromising other tests
// */
// class VerificationSpecificationTest : public VerificationSpecification
//{
// public:
//    static VerificationSpecificationTest& getInstance()
//    {
//        if(!instance_)
//            instance_ = new VerificationSpecificationTest;
//        return (*instance_);
//    }
//
//    void parse(std::string errorStr) { VerificationSpecification::parse(errorStr); }
//
// private:
//    static VerificationSpecificationTest* instance_;
//};
// VerificationSpecificationTest* VerificationSpecificationTest::instance_ = nullptr;
//
// TEST(Verification, VerificationReporter)
//{
//    VerificationSpecificationTest& reporter = VerificationSpecificationTest::getInstance();
//
//    // Keyword: field
//    reporter.parse("field=foo");
//    ASSERT_STREQ(reporter.fieldname().c_str(), "foo");
//
//    // Keyword: visualize
//    reporter.parse("visualize");
//    ASSERT_TRUE(reporter.visualize());
//
//    // Keyword: list
//    reporter.parse("list");
//    ASSERT_TRUE(reporter.list());
//    ASSERT_GT(reporter.maxErrorsToList(), 0);
//
//    // Keyword: stop-on-error
//    reporter.parse("stop-on-error");
//    ASSERT_TRUE(reporter.stopOnError());
//
//    // Keyword: max-errors
//    reporter.parse("max-errors=100");
//    ASSERT_EQ(reporter.maxErrorsToList(), 100);
//    ASSERT_TRUE(reporter.list());
//
//    // Keyword: k
//    reporter.parse("k=5");
//    ASSERT_TRUE(reporter.kIntervalSpecified());
//    ASSERT_EQ(reporter.kInterval()[0], 5);
//
//    reporter.parse("k=0");
//    ASSERT_TRUE(reporter.kIntervalSpecified());
//    ASSERT_EQ(reporter.kInterval()[0], 0);
//
//    reporter.parse("k=5-10");
//    ASSERT_THAT(reporter.kInterval(), testing::ElementsAre(5, 6, 7, 8, 9, 10));
//
//    reporter.parse("k=5-7,k=12");
//    ASSERT_THAT(reporter.kInterval(), testing::ElementsAre(5, 6, 7, 12));
//
//    // Multiple keywords
//    reporter.parse("visualize,list");
//    ASSERT_FALSE(reporter.kIntervalSpecified());
//    ASSERT_TRUE(reporter.list());
//    ASSERT_TRUE(reporter.visualize());
//}
