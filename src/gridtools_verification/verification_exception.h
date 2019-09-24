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

#include "core/include_boost_format.h"
#include <stdexcept>
#include "common.h"

namespace gt_verification {

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
}
