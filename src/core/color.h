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
        bool usecoloredOutput(FILE *stream);
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
        if (internal::usecoloredOutput(stream))
            std::fprintf(stream, "\033[0;%sm", internal::colorToVT100(color));

        std::fprintf(stream, fmt, args...);

        if (internal::usecoloredOutput(stream))
            std::fprintf(stream, "\033[m");
    }

    inline void cfprintf(FILE *stream, color color, const char *fmt) {
        if (internal::usecoloredOutput(stream))
            std::fprintf(stream, "\033[0;%sm", internal::colorToVT100(color));

        std::fputs(fmt, stream);

        if (internal::usecoloredOutput(stream))
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
