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

#include <vector>
#include <string>
#include "../common.h"
#include "../core/command_line.h"
#include "verification.h"

namespace gt_verification {

    /**
     * @brief Specify the behaviour how errors are being reported
     *
     * The information is passed via command_line (`--error`) as a single string of a comma seperated
     * list of `keyword-value` pairs.
     * Use `./DycoreUnittest --error=help` for a detailed description on the keyword-value pairs.
     *
     * @b Example:
     * @code
     * ./DycoreUnittest --error=visualize,k=1-5,k=57
     * @endcode
     *
     * @ingroup DycoreUnittestVerificationLibrary
     */
    class verification_specification {
      protected:
      public:
        verification_specification(command_line &cl);
        void parse(std::string errorStr);

        /**
         * @brief Print the help information about the keywords and exit the program with
         *        EXIT_SUCCESS(0)
         */
        static void print_help(char *currentExecutable) noexcept;

        /**
         * @brief Only report field with specified name
         *
         * If the string is empty, all erroneous fields are being reported.
         *
         * @code
         * ./DycoreUnittest --error=field=u_nnow
         * @endcode
         */
        std::string fieldname() const noexcept { return fieldname_; }

        /**
         * @brief Whether to report the failures in the format ([i,j,k] | Actual | Refrence)
         *
         * To control the number of errors being reported maxErrorsToList() and to specify the
         * layers being reported kInterval().
         *
         * @code
         * ./DycoreUnittest --error=list
         * @endcode
         *
         * @see VerificationSpecification::maxErrorsToList()
         * @see VerificationSpecification::kInterval()
         */
        bool list() const noexcept { return list_; }

        /**
         * @brief Only report errors for one field and abort execution afterwards.
         *
         * @code
         * ./DycoreUnittest --error=stop-on-error
         * @endcode
         */
        bool stop_on_error() const noexcept { return stopOnError_; }

        /**
         * @brief Visualize the layers in ASCII art.
         *
         * To control the layers being reported set kInterval().
         *
         * @code
         * ./DycoreUnittest --error=visualize
         * @endcode
         *
         * @see VerificationSpecification::kInterval()
         */
        bool visualize() const noexcept { return visualize_; }

        /**
         * @brief Only list the first N failures
         *
         * This implies list().
         *
         * @code
         * ./DycoreUnittest --error=max-errors=512
         * @endcode
         *
         * @see VerificationSpecification::list()
         */
        int max_errors_to_list() const noexcept { return maxErrorsToList_; }

        /**
         * @brief Specify the layers (k-direction) which are going to be reported
         *
         * Only report failures from the layers in the range [X, Y] where <X> and <Y> are two integers
         * seperated by '-' in the range [0, kmax]. If <Y> is omitted then only the layer <X> is being
         * reported. Example: k=5-10 or k=20.
         *
         * @code
         * ./DycoreUnittest --error=k=0-20
         * @endcode
         */
        const std::vector< int > &k_interval() const noexcept { return kInterval_; }

        /**
         * @brief Check whether a k interval was specified
         */
        bool k_interval_specified() const noexcept { return kIntervalSpecified_; }

      private:
        // Parsed options
        std::string fieldname_;        ///< Keyword: field
        bool list_;                    ///< Keyword: list
        bool stopOnError_;             ///< Keyword: stop-on-error
        bool visualize_;               ///< Keyword: visualize
        int maxErrorsToList_;          ///< Keyword: max-errors
        std::vector< int > kInterval_; ///< Keyword: k

        // Derived options
        bool kIntervalSpecified_;
    };
}
