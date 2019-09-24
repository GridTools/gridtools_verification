/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "color.h"

#include <boost/algorithm/string/predicate.hpp>
#include <gtest/gtest.h>

namespace gt_verification {

    namespace internal {

        const char *colorToVT100(color color_) {
            switch (color_) {
            case color::RED:
                return "31";
            case color::GREEN:
                return "32";
            case color::YELLOW:
                return "33";
            case color::BOLDWHITE:
                return "01";
            default:
                return "00";
            }
        }

        bool useColoredOutput(FILE *stream) {
#ifndef DYCORE_NO_COLOR
            auto useColor = testing::GTEST_FLAG(color);

            if (boost::algorithm::iequals(useColor, "auto")) {

                int fd = fileno(stream);
                bool isAtty = isatty(fd);

                const char *const term = getenv("TERM");

                // It may happen that TERM is undefined, then just cross fingers
                if (term == NULL)
                    return isAtty;

                return isAtty && ((strcmp(term, "xterm") == 0) || (strcmp(term, "xterm-color") == 0) ||
                                     (strcmp(term, "xterm-256color") == 0) || (strcmp(term, "screen") == 0) ||
                                     (strcmp(term, "screen-256color") == 0) || (strcmp(term, "rxvt-unicode") == 0) ||
                                     (strcmp(term, "rxvt-unicode-256color") == 0) || (strcmp(term, "linux") == 0) ||
                                     (strcmp(term, "cygwin") == 0) || (strcmp(term, "tmux") == 0) ||
                                     (strcmp(term, "tmux-256color") == 0));
            }
            return boost::algorithm::iequals(useColor, "yes") || boost::algorithm::iequals(useColor, "true") ||
                   boost::algorithm::iequals(useColor, "t") || boost::algorithm::iequals(useColor, "1");
#else
            return false;
#endif
        }

    } // namespace internal
} // namespace gt_verification
