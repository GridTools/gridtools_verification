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
#include <boost/tokenizer.hpp>
#include "utility.h"

namespace gt_verification {

std::string split_string(const std::string &str, std::size_t lineSize, std::size_t indentSize) noexcept {
    // Tokenize string
    std::vector< std::string > tokens(tokenize_string(str, " "));

    std::string resultString(indentSize, ' ');
    resultString += tokens[0];
    std::size_t curLineSize = resultString.size();

    for (std::size_t i = 1; i < tokens.size(); ++i) {
        curLineSize += tokens[i].size() + 1;

        // Skip to new line?
        if (curLineSize > lineSize) {
            resultString += '\n';
            curLineSize = tokens[i].size() + 1;

            // Indent new line?
            if (indentSize > 0) {
                resultString += std::string(indentSize, ' ');
                curLineSize += indentSize;
            }
        } else
            resultString += " ";

        // Append string
        resultString += tokens[i];
    }
    return resultString;
}

std::vector< std::string > tokenize_string(const std::string &str, std::string delim) noexcept {
    std::vector< std::string > tokensVector;

#ifdef DYCORE_PLATFORM_APPLE
    // Apparently boost::tokenizer has some issues on Mac OSX with certain characters, so
    // we fall back to a hand-crafted version
    std::size_t curPos = 0, actualPos = 0, delimPos = std::string::npos;
    while (true) {
        for (std::size_t i = 0; i < delim.size(); ++i)
            delimPos = std::min(delimPos, str.find_first_of(delim[i], curPos + 1));

        if (delimPos != std::string::npos) {
            actualPos = curPos == 0 ? 0 : curPos + 1;
            tokensVector.push_back(str.substr(actualPos, delimPos - actualPos));
            curPos = delimPos;
            delimPos = std::string::npos;
        } else
            break;
    }
    tokensVector.push_back(str.substr(curPos == 0 ? 0 : curPos + 1));
#else
    boost::char_separator< char > seperator(delim.c_str(), "", boost::drop_empty_tokens);
    boost::tokenizer< boost::char_separator< char > > tokens(str, seperator);
    tokensVector.insert(tokensVector.begin(), tokens.begin(), tokens.end());
#endif
    return tokensVector;
}
}
