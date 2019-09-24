/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// TODO recover test without dycore dependency

#include "../helper_dycore.h"
#include <gmock/gmock.h>
#include <gridtools_verification/verification/field_collection.h>
#include <gridtools_verification/verification/verification.h>
#include <gridtools_verification/verification/verification_specification.h>

using namespace gt_verification;

#ifdef HAS_GRIDTOOLS

class test_Verification : public ::testing::Test {
  protected:
    int iSize = 33;
    int jSize = 22;
    int kSize = 80;

    IJKStorageInfoType metaData;
    IJKRealField outField;
    IJKRealField refField;

    type_erased_field_view< Real > outView;
    type_erased_field_view< Real > refView;

    test_Verification()
        : metaData(iSize, jSize, kSize), outField(metaData, -1, "output"), refField(metaData, -1, "reference"),
          outView(outField), refView(refField) {
#ifdef DYCORE_USE_GPU
        outField.h2d_update();
        refField.h2d_update();
#endif
    }
};

TEST_F(test_Verification, UnmodifiedFieldShouldPass) {
    error_metric< Real > errorMetric(1e-6, 1e-8);

    verification< Real > test(outView, refView);
    ASSERT_TRUE(test.verify(errorMetric).passed());
}

TEST_F(test_Verification, ModifiedFieldShouldFail) {
    error_metric< Real > errorMetric(1e-6, 1e-8);

// The verifier is responsible for updating to host
#ifdef DYCORE_USE_GPU
    outField.d2h_update();
#endif
    outView(5, 5, 5) = 2;
#ifdef DYCORE_USE_GPU
    outField.h2d_update();
#endif

    verification< Real > test(outView, refView);
    ASSERT_FALSE(test.verify(errorMetric).passed());
}

#endif
