/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
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
