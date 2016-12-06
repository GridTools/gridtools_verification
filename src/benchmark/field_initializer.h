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

#include <random>
#include "../core.h"
#include "../common.h"

GT_VERIFICATION_NAMESPACE_BEGIN

/**
 * @brief Initialize a @c gridtools fields with random data.
 *
 * A simple multiplicative congruential pseudo-random number generator (@c std::minstd_rand) is
 * used.
 *
 * @b Example: Fill @c u_nnow with random data
 * @code{.cpp}
 * auto fieldInitializer = BenchmarkEnvironment::getInstance().fieldInitializer();
 * fieldInitializer->init(repository_->u_nnow());
 * @endcode
 *
 * @ingroup DycoreUnittestBenchmarkLibrary
 */
template < typename T >
class field_initializer : private boost::noncopyable /* singleton */
{
  public:
    using generator_type = std::minstd_rand;

    field_initializer() : device_(), generator_(device_()) {}

    /**
     * @brief Initialize a @c gridtools field with random numbers of type T uniformly
     * distributed on the interval [a, b)
     *
     * @param a     Lower bound (inclusive)
     * @param b     Upper bound (exclusive)
     */
    template < class FieldType >
    void init(FieldType &field, T a = -10.0, T b = 10.0) noexcept {
        std::uniform_real_distribution< T > dist(a, b);
        type_erased_field_view fieldView(field);
        fieldView.update_host();

        const int iSizeHalo = fieldView.i_size();
        const int jSizeHalo = fieldView.j_size();
        const int kSize = fieldView.k_size();

        VERIFICATION_LOG() << boost::format("Initializing %-15s (%3i, %3i, %2i)") % fieldView.name() % iSizeHalo %
                                  jSizeHalo % kSize
                           << logger_action::endl;

#pragma omp parallel for
        for (int i = 0; i < iSizeHalo; ++i)
            for (int j = 0; j < jSizeHalo; ++j)
                for (int k = 0; k < kSize; ++k)
                    fieldView(i, j, k) = dist(generator_);
    }

    /**
     * @brief Generate a random number in [a, b)
     *
     * @param a     Lower bound (inclusive)
     * @param b     Upper bound (exclusive)
     */
    inline T rand(T a = 0.0, T b = 1.0) noexcept {
        std::uniform_real_distribution< T > dist(a, b);
        return dist(generator_);
    }

  private:
    std::random_device device_;
    generator_type generator_;
};

GT_VERIFICATION_NAMESPACE_END
