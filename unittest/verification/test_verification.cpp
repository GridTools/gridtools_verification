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
