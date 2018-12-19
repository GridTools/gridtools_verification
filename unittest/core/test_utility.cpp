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
#include <gmock/gmock.h>
#include <gridtools_verification/core/utility.h>

using namespace gt_verification;

TEST(test_Utility, split_string) {
    std::string str("The quick brown fox jumps over the lazy dog.");

    // Split over 20 charachters
    std::string resStr1(split_string(str, 20, 0));
    std::string refStr1("The quick brown fox\njumps over the lazy\ndog.");
    ASSERT_STREQ(resStr1.c_str(), refStr1.c_str());

    // Split over 25 characters and indent lines by 5 characters
    std::string resStr2(split_string(str, 25, 5));
    std::string refStr2("     The quick brown fox\n     jumps over the lazy\n     dog.");
    ASSERT_STREQ(resStr2.c_str(), refStr2.c_str());

    // Split over 80 characters (should do nothing here)
    std::string resStr3(split_string(str, 80, 0));
    ASSERT_STREQ(resStr3.c_str(), str.c_str());

    // Split over 80 characters and indent lines by 40
    std::string resStr4(split_string(str, 80, 40));
    std::string refStr4("                                        The quick brown fox jumps over "
                        "the lazy\n                                        dog.");
    ASSERT_STREQ(resStr4.c_str(), refStr4.c_str());

    // Split long words over 10
    std::string str5("ThisIsTooLongToBeSpread ThisIsAsWell");
    std::string refStr5("ThisIsTooLongToBeSpread\nThisIsAsWell");
    std::string resStr5(split_string(str5, 10, 0));
    ASSERT_STREQ(resStr5.c_str(), refStr5.c_str());
}

TEST(test_Utility, tokenize_string) {
    // Tokenize with ' '
    std::string str1("The quick brown fox jumps over the lazy dog.");
    ASSERT_THAT(tokenize_string(str1, " "),
        testing::ElementsAre("The", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog."));
    // Tokeninze with ','
    std::string str2("The,quick,brown,fox,   jumps, over, the,lazy , dog.");
    ASSERT_THAT(tokenize_string(str2, ","),
        testing::ElementsAre("The", "quick", "brown", "fox", "   jumps", " over", " the", "lazy ", " dog."));

    // Tokeninze with ',:|'
    std::string str3("The,quick:brown|fox");
    ASSERT_THAT(tokenize_string(str3, ",:|"), testing::ElementsAre("The", "quick", "brown", "fox"));
}
