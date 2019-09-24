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

#include <boost/program_options.hpp>
#include <string>
#include "../common.h"

namespace gt_verification {

    /**
     * @brief Handle command-line arguments
     *
     * This is just a simple wrapper around Boost.Program_options.
     *
     * @ingroup DycoreUnittestCoreLibrary
     */
    class command_line {

      public:
        command_line(int argc, const char *const argv[]);

        /**
         * @brief Print help information and exit the program with EXIT_SUCCESS(0)
         */
        void print_help(char *currentExecutable) const noexcept;

        /**
         * @brief Check whether the command-line argument @c arg has been passed
         */
        bool has(const std::string &arg) const { return (variableMap_.count(arg) == 1); }

        /**
         * @brief Check whether the command-line argument @c arg is still set to the default value
         */
        bool defaulted(const std::string &arg) const {
            return (variableMap_.count(arg) == 1) ? variableMap_[arg].defaulted() : false;
        }

        /**
         * @brief Get command-line argument @c arg as type @c ValueType
         *
         * @throw boost::bad_any_cast The requested type does not match the internal type of the value
         * @return Returns a copy of the held value
         */
        template < typename ValueType >
        ValueType as(const std::string &arg) const {
            return variableMap_[arg].as< ValueType >();
        }

      private:
        boost::program_options::variables_map variableMap_;
        boost::program_options::options_description desc_;
    };
}
