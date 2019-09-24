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

namespace gt_verification {

    template < typename T >
    class error_metric_interface {
      public:
        /**
         * @brief Check if two real numbers @c a and @c b are equal within a tolerance
         */
        virtual bool equal(T a, T b) const noexcept = 0;
    };
}
