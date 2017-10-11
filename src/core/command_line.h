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
