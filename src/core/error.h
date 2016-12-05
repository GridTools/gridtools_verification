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

#include <cstdlib>
#include <iostream>
#include "../common.h"

GT_VERIFICATION_NAMESPACE_BEGIN

/**
 * @brief Interface to handle runtime errors
 *
 * @ingroup DycoreUnittestCoreLibrary
 */
struct error : private boost::noncopyable {
    error() = delete;

    /**
     * @brief Print an error message to stderr and exit the program with EXIT_FAILURE(1)
     *
     * The error message will be formatted as "error: ErrorMessage"
     */
    template < class MessageType >
    static void fatal(MessageType &&ErrorMessage) noexcept {
        std::cerr << "error: " << ErrorMessage << std::endl;
        std::exit(EXIT_FAILURE);
    }

    /**
     * @brief Print a warning message to stdout
     *
     * The warning message will be formatted as "warning: WarningMessage"
     */
    template < class MessageType >
    static void warning(MessageType &&WarningMessage) noexcept {
        std::cout << "warning: " << WarningMessage << std::endl;
    }
};

GT_VERIFICATION_NAMESPACE_END
