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
#include "../Core.h"
#include "Common.h"

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
class FieldInitializer : private boost::noncopyable /* singleton */
{
  public:
    using GeneratorType = std::minstd_rand;

    FieldInitializer() : device_(), generator_(device_()) {}

    /**
     * @brief Initialize a @c gridtools field with @ref dycore::Real "Real" random numbers uniformly
     * distributed on the interval [a, b)
     *
     * @param a     Lower bound (inclusive)
     * @param b     Upper bound (exclusive)
     */
    template < class FieldType >
    void init(FieldType &field, Real a = -10.0, Real b = 10.0) noexcept {
        std::uniform_real_distribution< Real > dist(a, b);
        TypeErasedFieldView fieldView(field);
        fieldView.updateHost();

        const int iSizeHalo = fieldView.iSize();
        const int jSizeHalo = fieldView.jSize();
        const int kSize = fieldView.kSize();

        VERIFICATION_LOG() << boost::format("Initializing %-15s (%3i, %3i, %2i)") % fieldView.name() % iSizeHalo %
                                  jSizeHalo % kSize
                           << logger::endl;

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
    inline Real rand(Real a = 0.0, Real b = 1.0) noexcept {
        std::uniform_real_distribution< Real > dist(a, b);
        return dist(generator_);
    }

  private:
    std::random_device device_;
    GeneratorType generator_;
};

GT_VERIFICATION_NAMESPACE_END
