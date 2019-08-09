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
#include "../core/color.h"
#include "../core/command_line.h"
#include "../core/error.h"
#include "../core/include_boost_format.h"
#include "../core/utility.h"
#include "verification.h"
#include "verification_specification.h"
#include <cstdint>
#include <cstdlib>
#include <initializer_list>
#include <limits>
#include <numeric>
#include <string>
#include <vector>

namespace gt_verification {

    class error_layer {
      public:
        template < typename failures_t >
        error_layer(int size_i, int size_j, failures_t const &failures)
            : size_i_{size_i}, size_j_{size_j}, error_mask_(size_i * size_j, false) {

            for (auto const &failure : failures) {
                mark(failure.i, failure.j);
            }
        }

        bool get(int i, int j) const { return error_mask_[i * size_j_ + j]; }
        int size_i() const { return size_i_; }
        int size_j() const { return size_j_; }

      private:
        int size_i_;
        int size_j_;
        std::vector< bool > error_mask_;

        void mark(int i, int j) { error_mask_[i * size_j_ + j] = true; }
    };

    // Print a layer
    template < typename failures_t >
    void printLayer(error_layer const &layer, failures_t const &failures, int k, std::string const &field_name) {
        std::printf("\nk = %i (%s)\n\n       j\n   0-------->\n", k, field_name.c_str());

        // First failure in this layer (this is used to print some (i,j,k) triplets
        // on the right hand side)
        auto it = failures.cbegin();

        for (int i = 0; i < layer.size_i(); ++i) {
            // Print left side of the row (arrow in i-direction)
            if (i < 4)
                std::printf(i == 3 ? "   v " : (i == 1 ? " i | " : "   | "));
            else
                std::printf("     ");

            // Print a row (in color)
            for (int j = 0; j < layer.size_j(); ++j)
                if (layer.get(i, j))
                    cprintf(color::RED, "X ");
                else
                    cprintf(color::GREEN, "X ");

            // Print right hand side
            if (it != failures.cend()) {
                std::printf(" (%3i,%3i,%3i)\n", it->i, it->j, it->k);
                ++it;
            } else
                std::printf("\n");
        }
        std::printf("\n");
    };

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
            type_erased_field_view< T > referenceField = verif.reference_field();
            type_erased_field_view< T > outputField = verif.output_field();

            // If the interval is not specified, we will print everything. Note: this may trigger some
            // unnecessary copies but it doesn't really matter here.
            std::vector< int > kInterval;
            if (!verifSpec_.k_interval_specified()) {
                kInterval.resize(referenceField.k_size());
                std::iota(kInterval.begin(), kInterval.end(), 0);
            } else
                kInterval = verifSpec_.k_interval();

            // Iterate over the specified layers (k-direction)
            for (auto k : kInterval) {

                std::vector< typename gt_verification::verification< T >::failure > k_failures;
                std::copy_if(failures.cbegin(),
                    failures.cend(),
                    std::back_inserter(k_failures),
                    [k](typename gt_verification::verification< T >::failure const &f) { return f.k == k; });

                if (k_failures.size() > 0) {
                    error_layer layer{referenceField.i_size(), referenceField.j_size(), k_failures};
                    printLayer(layer, k_failures, k, outputField.name());
                }
            }
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
} // namespace gt_verification
