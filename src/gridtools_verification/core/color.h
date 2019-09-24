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

#include "../common.h"
#include <string>
#include <string.h>
#include <cstdio>
#include <cstdlib>

namespace gt_verification {

    /**
     * @defgroup colors
     * @brief colored terminal support via dycore::cfprintf and dycore::cprintf.
     *
     * @b Example:
     * @code{.cpp}
     * cprintf(color::RED, "This is red!");
     * cprintf(color::BOLDWHITE, "The answer is %i", 42);
     * @endcode
     *
     * @ingroup DycoreUnittestCoreLibrary
     * @{
     */

    /**
     * @brief colors used by cfprintf and cprintf
     */
    enum class color { INVALID = 0, RED, GREEN, YELLOW, BOLDWHITE };

    namespace internal {

        /**
         * Map dycore::color to ANSI/VT100 console color codes
         */
        const char *colorToVT100(color color);

        /**
         * Check whether we can use colored output or not
         */
        bool useColoredOutput(FILE *stream);
    }

    /**
     * @brief fprintf-like function with support for colored output
     *
     * This function will inject ANSI/VT100 console color codes to change the terminal color.
     *
     * @b Example:
     * @code{.cpp}
     * cprintf(color::RED, "This is red!");
     * cprintf(color::BOLDWHITE, "The answer is %i", 42);
     * @endcode
     *
     * @{
     */
    template < typename... Args >
    inline void cfprintf(FILE *stream, color color, const char *fmt, Args &&... args) {
        if (internal::useColoredOutput(stream))
            std::fprintf(stream, "\033[0;%sm", internal::colorToVT100(color));

        std::fprintf(stream, fmt, args...);

        if (internal::useColoredOutput(stream))
            std::fprintf(stream, "\033[m");
    }

    inline void cfprintf(FILE *stream, color color, const char *fmt) {
        if (internal::useColoredOutput(stream))
            std::fprintf(stream, "\033[0;%sm", internal::colorToVT100(color));

        std::fputs(fmt, stream);

        if (internal::useColoredOutput(stream))
            std::fprintf(stream, "\033[m");
    }
    /** @} */

    /**
     * @brief printf-like function with support for colored output
     *
     * @see dycore::cfprintf
     */
    template < typename... Args >
    inline void cprintf(color color, const char *fmt, Args &&... args) {
        cfprintf(stdout, color, fmt, std::forward< Args >(args)...);
    }

    /** @} */
}
