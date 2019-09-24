/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <gmock/gmock.h>
#include <gridtools_verification/verification/error_metric.h>

TEST(error_metric, clear_separation) {
    gt_verification::error_metric< float > em1(1.e-6, 1e-8);
    ASSERT_TRUE(em1.equal(1.0, 1.0));
    ASSERT_FALSE(em1.equal(1.0, 2.0));
}
