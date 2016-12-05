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

// FIXME Code duplication with the dycore
#pragma once

#include <boost/format.hpp>
#include <stdexcept>
#include "common.h"

GT_VERIFICATION_NAMESPACE_BEGIN

namespace {

    /**
     * Expand variadic arguments
     */
    template < typename... Args >
    std::string format_vargs(std::string const &fmt, Args &&... args) {
        boost::format f(fmt);
        int unroll[]{0, (f % std::forward< Args >(args), 0)...};
        static_cast< void >(unroll);
        return boost::str(f);
    }
}

/**
 * @brief Simple exception class which stores a human-readable error description
 *
 * Creates a @c std::runtime_error with @c std::runtime_error::what() set to the formatted
 * string.
 *
 * @code{.cpp}
 * try
 * {
 *     throw Exception("Nope! The answer should be %i", 42);
 * }
 * catch(Exception& e)
 * {
 *    std::cout << "Caught a dycore::Exception saying: " << e.what() << std::endl;
 * }
 * @endcode
 *
 */
class verification_exception : public std::runtime_error {
  public:
    /**
     * @brief Variadic constructor to support printf-style arguments
     *
     * @param fmt   Printf like format string
     * @param args  Arguments specifying data to print
     */
    template < typename... Args >
    verification_exception(const char *fmt, const Args &... args)
        : std::runtime_error(format_vargs(fmt, args...)) {}
};

GT_VERIFICATION_NAMESPACE_END
