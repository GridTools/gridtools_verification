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

#include <cmath>
#include "../common.h"
#include "error_metric_interface.h"

namespace gt_verification {

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
    class error_metric : public error_metric_interface< T > {
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
        bool equal(T a, T b) const noexcept override { return (std::fabs(a - b) <= (atol_ + rtol_ * std::fabs(b))); }

      private:
        T rtol_;
        T atol_;
    };
}
