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

#include <cmath>
#include "../common.h"

GT_VERIFICATION_NAMESPACE_BEGIN

/**
 * @brief Define a metric to compare real numbers within the given absolute and relative tolerance
 *
 * The tolerance values are positive, typically very small numbers. To compare two real numbers a
 * and b, the relative difference (rtol * abs(b)) and the absolute difference atol are added
 * together to compare against the absolute difference between a and b.
 *
 * @f[
 * |a - b| \leq (\texttt{atol} + \texttt{rtol} \cdot |b|)
 * @f]
 *
 * @ingroup DycoreUnittestVerificationLibrary
 */
template < typename T >
class error_metric {
  public:
    error_metric(const error_metric &) = default;
    error_metric &operator=(const error_metric &) = default;

    /**
     * @brief Initialize the ErrorMetric with the relative and absolute tolerance
     *
     * @param rtol  Relative tolerance
     * @param atol  Absolute tolerance
     */
    error_metric(T rtol, T atol) : rtol_(rtol), atol_(atol) {}

    /**
     * @brief Check if two real numbers @c a and @c b are equal within a tolerance
     *
     * @return true iff absolute(a - b) <= (atol + rtol * absolute(b))
     */
    inline bool equal(T a, T b) const noexcept { return (std::fabs(a - b) <= (atol_ + rtol_ * std::fabs(b))); }

  private:
    T rtol_;
    T atol_;
};

GT_VERIFICATION_NAMESPACE_END
