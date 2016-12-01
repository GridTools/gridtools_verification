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
#include "Color.h"

GT_VERIFICATION_NAMESPACE_BEGIN

namespace internal {

    const char *colorToVT100(Color color) {
        switch (color) {
        case Color::RED:
            return "31";
        case Color::GREEN:
            return "32";
        case Color::YELLOW:
            return "33";
        case Color::BOLDWHITE:
            return "01";
        default:
            return "00";
        }
    }

    bool useColoredOutput(FILE *stream) {
#ifndef DYCORE_NO_COLOR
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
                             (strcmp(term, "cygwin") == 0));
#else
        return false;
#endif
    }
}

GT_VERIFICATION_NAMESPACE_END
