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

//#include <gmock/gmock.h>
//#include <core/TypeErasedField.h>
//#include <dycore/Storage.h> // FIXME
//
// using namespace gt_verification;
// using namespace dycore; // FIXME
//
// class testsetup_TypeErasedField : public ::testing::Test {
//  protected:
//    const int iSize = 33;
//    const int jSize = 22;
//    const int kSize = 80;
//
//    const int iSizeNoHalo = iSize - dycore::iPlusHaloSize - dycore::iMinusHaloSize;
//    const int jSizeNoHalo = jSize - dycore::jPlusHaloSize - dycore::jMinusHaloSize;
//
//    IJKMetaStorageType metaData;
//    IJKRealField field;
//
//    template < typename Field, typename Function >
//    void initHelper(Field &field, Function &&f) {
//        for (int i = 0; i < iSize; ++i)
//            for (int j = 0; j < jSize; ++j)
//                for (int k = 0; k < kSize; ++k) {
//                    field(i, j, k) = f(i, j, k);
//                }
//    }
//
//    testsetup_TypeErasedField() : metaData(iSize, jSize, kSize), field(metaData, 0, "field") {
//        initHelper(field, [](int i, int j, int k) { return i * j * k; });
//    }
//};
//
///**
// * TODO not sure why we want to test a const and a non-const version, the original test case was
// * already broken...
// */
// class test_TypeErasedFieldViewConstant : public testsetup_TypeErasedField {
//  protected:
//    const type_erased_field_view fieldView_c;
//    test_TypeErasedFieldViewConstant() : fieldView_c(field) {}
//};
//
// TEST_F(test_TypeErasedFieldViewConstant, CheckSize) {
//    ASSERT_EQ(fieldView_c.i_size(), iSize);
//    ASSERT_EQ(fieldView_c.i_size_no_halo(), iSizeNoHalo);
//
//    ASSERT_EQ(fieldView_c.j_size(), jSize);
//    ASSERT_EQ(fieldView_c.j_size_no_halo(), jSizeNoHalo);
//
//    ASSERT_EQ(fieldView_c.k_size(), kSize);
//}
//
// TEST_F(test_TypeErasedFieldViewConstant, CheckFieldName) { ASSERT_STREQ(fieldView_c.name().c_str(), "field"); }
//
// TEST_F(test_TypeErasedFieldViewConstant, CheckFieldEntries) {
//    for (int i = 0; i < iSize; ++i)
//        for (int j = 0; j < jSize; ++j)
//            for (int k = 0; k < kSize; ++k)
//                ASSERT_REAL_EQ(fieldView_c(i, j, k), i * j * k);
//}
//
// class test_TypeErasedFieldViewNonConstant : public testsetup_TypeErasedField {
//  protected:
//    type_erased_field_view fieldView_nc;
//    test_TypeErasedFieldViewNonConstant() : fieldView_nc(field) {}
//};
//
// TEST_F(test_TypeErasedFieldViewNonConstant, ModificationAffectsOriginalField) {
//    // Modify elements
//    for (int i = 0; i < iSize; ++i)
//        for (int j = 0; j < jSize; ++j)
//            for (int k = 0; k < kSize; ++k)
//                fieldView_nc(i, j, k) = i * j * k + 1;
//
//    for (int i = 0; i < iSize; ++i)
//        for (int j = 0; j < jSize; ++j)
//            for (int k = 0; k < kSize; ++k)
//                ASSERT_REAL_EQ(field(i, j, k), i * j * k + 1);
//}
//
// class test_TypeErasedField : public testsetup_TypeErasedField {
//  protected:
//    type_erased_field field_te;
//    test_TypeErasedField() : field_te(field){};
//};
//
// TEST_F(test_TypeErasedField, CheckSizes) {
//    ASSERT_EQ(field_te.i_size(), iSize);
//    ASSERT_EQ(field_te.i_size_no_halo(), iSizeNoHalo);
//
//    ASSERT_EQ(field_te.j_size(), jSize);
//    ASSERT_EQ(field_te.j_size_no_halo(), jSizeNoHalo);
//
//    ASSERT_EQ(field_te.k_size(), kSize);
//}
//
// TEST_F(test_TypeErasedField, CheckFieldName) { ASSERT_STREQ(field_te.name().c_str(), "field"); }
//
// TEST_F(test_TypeErasedField, CheckFieldEntries) {
//    for (int i = 0; i < iSize; ++i)
//        for (int j = 0; j < jSize; ++j)
//            for (int k = 0; k < kSize; ++k)
//                ASSERT_REAL_EQ(field_te(i, j, k), field(i, j, k));
//}
//
// TEST_F(test_TypeErasedField, ModificationDoesNotAffectOriginalField) {
//    for (int i = 0; i < iSize; ++i)
//        for (int j = 0; j < jSize; ++j)
//            for (int k = 0; k < kSize; ++k)
//                field_te(i, j, k) = i * j * k + 1;
//
//    for (int i = 0; i < iSize; ++i)
//        for (int j = 0; j < jSize; ++j)
//            for (int k = 0; k < kSize; ++k)
//                ASSERT_REAL_EQ(field(i, j, k), i * j * k);
//}
//
// TEST_F(test_TypeErasedField, ModificationOfOriginalFieldDoesNotAffectTypeErasedField) {
//    // Modify original field
//    for (int i = 0; i < iSize; ++i)
//        for (int j = 0; j < jSize; ++j)
//            for (int k = 0; k < kSize; ++k)
//                field(i, j, k) = i * j * k + 2;
//
//    // Check type erased elements haven't changed
//    for (int i = 0; i < iSize; ++i)
//        for (int j = 0; j < jSize; ++j)
//            for (int k = 0; k < kSize; ++k)
//                ASSERT_REAL_EQ(field_te(i, j, k), i * j * k);
//}
//
// TEST_F(test_TypeErasedField, TypeErasedFieldToView) {
//    type_erased_field_view field_te_view(field_te.to_view());
//
//    for (int i = 0; i < iSize; ++i)
//        for (int j = 0; j < jSize; ++j)
//            for (int k = 0; k < kSize; ++k)
//                ASSERT_REAL_EQ(field_te_view(i, j, k), field_te(i, j, k));
//}
