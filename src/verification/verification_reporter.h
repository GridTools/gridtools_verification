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
#include <cstdint>
#include <cstdlib>
#include <initializer_list>
#include <limits>
#include <numeric>
#include <boost/format.hpp>
#include "common.h"
#include "verification_specification.h"
#include "verification.h"
#include "../core/color.h"
#include "../core/command_line.h"
#include "../core/error.h"
#include "../core/utility.h"
#include "error_metric.h"

GT_VERIFICATION_NAMESPACE_BEGIN

/**
 * @brief Report errors to console
 *
 * @ingroup DycoreUnittestVerificationLibrary
 */
class verification_reporter : private boost::noncopyable {
  protected:
    template < typename T >
    void list_failures(const verification< T > &verif) const noexcept {
        if (!verifSpec_.fieldname().empty() && verif.output_field().name() != verifSpec_.fieldname())
            return;

        const auto &failures = verif.failures();
        int curErrors = 0;

        if (!failures.empty()) {
            // Print header
            std::cout << boost::format("%13s | %24s | %24s\n") % "Position" %
                             (boost::format("Actual [%s]") % verif.output_field().name()).str() % "Reference";
            std::cout << std::string(67, '-') << "\n";

            for (const auto &fail : failures) {
                // Check if we only print from a specific k-layer (this is definitly not the smartest
                // way of doing this :)
                if (verifSpec_.k_interval_specified())
                    if (std::count(verifSpec_.k_interval().begin(), verifSpec_.k_interval().end(), fail.k) == 0)
                        continue;

                // Print errors (no more than maxErrorsToList)
                if (curErrors++ < verifSpec_.max_errors_to_list())
                    std::cout << boost::format("(%3i,%3i,%3i) | %24.12f | %24.12f\n") % fail.i % fail.j % fail.k %
                                     fail.outVal % fail.refVal;
            }
            std::cout << std::endl;
        }
    }

    template < typename T >
    void visualize_failures(const verification< T > &verif) const noexcept {
        if (!verifSpec_.fieldname().empty() && verif.output_field().name() != verifSpec_.fieldname())
            return;

        const auto &failures = verif.failures();
        type_erased_field_view referenceField = verif.reference_field();
        type_erased_field_view outputField = verif.output_field();

        // If the interval is not specified, we will print everything. Note: this may trigger some
        // unnecessary copies but it doesn't really matter here.
        std::vector< int > kInterval;
        if (!verifSpec_.k_interval_specified()) {
            kInterval.resize(referenceField.k_size());
            std::iota(kInterval.begin(), kInterval.end(), 0);
        } else
            kInterval = verifSpec_.k_interval();

        // The boolean vector of the current layer (true == failure)
        const int N = referenceField.i_size();
        const int M = referenceField.j_size();
        std::vector< bool > layer(N * M, false);
        int layerK = -1;

        // Print a layer (given by index k)
        auto printLayer = [&](int k) {
            // Search for the first failure in this layer (this is used to print some (i,j,k) triplets
            // on the right hand side)
            auto it = failures.cbegin();
            while (it != failures.cend() && it->k != k)
                ++it;

            std::printf("\nk = %i (%s)\n\n       j\n   0-------->\n", k, outputField.name().c_str());
            for (int i = 0; i < N; ++i) {
                // Print left side of the row (arrow in i-direction)
                if (i < 4)
                    std::printf(i == 3 ? "   v " : (i == 1 ? " i | " : "   | "));
                else
                    std::printf("     ");

                // Print a row (in color)
                for (int j = 0; j < M; ++j)
                    if (layer[i * M + j])
                        cprintf(color::RED, "X ");
                    else
                        cprintf(color::GREEN, "X ");

                // Print right hand side
                if (it != failures.cend() && (it->k == k)) {
                    std::printf(" (%3i,%3i,%3i)\n", it->i, it->j, it->k);
                    ++it;
                } else
                    std::printf("\n");
            }
            std::printf("\n");
        };

        auto failureIt = failures.cbegin();
        bool hasError = false;

        // Iterate over the specified layers (k-direction)
        for (auto k : kInterval) {
            for (; failureIt != failures.cend(); ++failureIt) {
                if (failureIt->k == k) {
                    layer[failureIt->i * M + failureIt->j] = true;
                    layerK = k;
                    hasError = true;
                } else if (failureIt->k > k) {
                    if (hasError) {
                        // Print the layer & reset it
                        printLayer(layerK);
                        std::fill(layer.begin(), layer.end(), false);
                        hasError = false;
                    }
                    break;
                }
            }
        }

        // In case only one layer has an error, we never go into the else if branch, hence we print it
        // here
        if (hasError)
            printLayer(layerK);
    }

  public:
    verification_reporter(const verification_specification verifSpec) : verifSpec_(verifSpec){};

    /**
     * @brief Report failures to console
     *
     * The behaviour depends on the passed error string to the command-line option @c --error.
     *
     * @param verification  The verification object storing potential failures
     *
     * @see VerificationSpecification
     */
    template < typename T >
    void report(const verification< T > &verif) const noexcept {
        if (verifSpec_.list())
            list_failures(verif);

        if (verifSpec_.visualize())
            visualize_failures(verif);

        // If no field is specified, we exit on first error
        if (verifSpec_.stop_on_error() &&
            (verifSpec_.fieldname().empty() || verifSpec_.fieldname() == verif.output_field().name()))
            std::exit(EXIT_FAILURE);
    }

  private:
    verification_specification verifSpec_;
};

GT_VERIFICATION_NAMESPACE_END
