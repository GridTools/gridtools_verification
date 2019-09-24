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

#include <cstdlib>
#include <iostream>
#include "../common.h"

namespace gt_verification {

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
}
