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

#include <core.h>
#include <core/serialization.h>
#include <cmath>
#include "helper_dycore.h"

using namespace gt_verification;

#ifdef HAS_GRIDTOOLS
#include <gridtools.hpp>
/**
 * @brief Serialization unittest
 */
class SerializationUnittest : public ::testing::Test {
  public:
    /**
     * Fill GridTools field with unique values
     */
    template < class FieldType >
    static void fillUniqueValues(FieldType &field) {
        field.sync();
        auto view = make_host_view(field);
        for (unsigned i = 0; i < iSize; ++i)
            for (unsigned j = 0; j < jSize; ++j)
                for (unsigned k = 0; k < kSize; ++k)
                    view(i, j, k) = i + j + k;
        field.sync();
    }

    SerializationUnittest() : savepoint_("TestPoint") {
        serializationUnittestSerializer_ =
            std::make_shared< ser::serializer >(ser::open_mode::Write, ".", "SerializationUnittest");

        serialization_ = std::make_shared< serialization >(serializationUnittestSerializer_);
        files_.clear();
        files_.push_back("SerializationUnittest.json");
    }

  protected:
    /// SerialBox objects
    std::shared_ptr< ser::serializer > serializationUnittestSerializer_;
    ser::savepoint savepoint_;

    /// Serialization, contains the load and write methods
    std::shared_ptr< serialization > serialization_;

    /// Record the files we created (to cleanup in the end)
    std::vector< std::string > files_;

    /// Sizes of the fields
    static constexpr int iSize = 33;
    static constexpr int jSize = 22;
    static constexpr int kSize = 80;

    virtual void TearDown() override {
        for (const auto &file : files_)
            if (remove(file.c_str()) != 0)
                VERIFICATION_LOG() << "failed to remove file '" << file << "'" << logger_action::endl;
        files_.clear();
    }
};

/**
 * Serialize a GridTools field to disk and load it again
 */
TEST_F(SerializationUnittest, GridToolsToGridTools) {
    IJKStorageInfoType metaData(iSize, jSize, kSize);

    IJKRealField gridToolsField1(metaData, -1, "GridToolsField1");
    fillUniqueValues(gridToolsField1);

    // Write to disk
    serialization_->write_gt("GridToolsField", gridToolsField1, savepoint_);
    files_.push_back("SerializationUnittest_GridToolsField.dat");

    // Load from disk
    IJKRealField gridToolsField2(metaData, -1, "GridToolsField2");
    serialization_->load_gt("GridToolsField", gridToolsField2, savepoint_);

    auto view1 = make_host_view(gridToolsField1);
    auto view2 = make_host_view(gridToolsField2);

    // Verify result
    for (int i = 0; i < iSize; ++i)
        for (int j = 0; j < jSize; ++j)
            for (int k = 0; k < kSize; ++k)
                ASSERT_REAL_EQ(view2(i, j, k), view1(i, j, k));
}

/**
 * Serialize a "fake" fortran field to disk and load it into a GridTools field
 */
TEST_F(SerializationUnittest, FortranToGridTools) {
    // Fortran field (Column-major)
    Real fortranField[iSize * jSize * kSize];

    for (int k = 0; k < kSize; ++k)
        for (int j = 0; j < jSize; ++j)
            for (int i = 0; i < iSize; ++i)
                fortranField[k * jSize * iSize + j * iSize + i] = i + j + k;

    // Register field using SerialBox serializer
    const int bytesPerElement = sizeof(Real);

    auto typeID = serialbox::ToTypeID< Real >::value;

    std::vector< int > dims{iSize, jSize, kSize};

    ser::meta_info_map metaInfo;
    metaInfo.insert("__name", "FortranField");
    metaInfo.insert("__elementtype", serialbox::TypeUtil::toString(typeID));
    metaInfo.insert("__bytesperelement", bytesPerElement);
    metaInfo.insert("__rank", (int)dims.size());
    metaInfo.insert("__isize", dims[0]);
    metaInfo.insert("__jsize", dims[1]);
    metaInfo.insert("__ksize", dims[2]);
    metaInfo.insert("__lsize", 1);
    metaInfo.insert("__iminushalosize", iMinusHaloSize);
    metaInfo.insert("__iplushalosize", iPlusHaloSize);
    metaInfo.insert("__jminushalosize", jMinusHaloSize);
    metaInfo.insert("__jplushalosize", jPlusHaloSize);
    metaInfo.insert("__kminushalosize", 0);
    metaInfo.insert("__kplushalosize", 0);
    metaInfo.insert("__lminushalosize", 0);
    metaInfo.insert("__lplushalosize", 0);

    std::vector< int > strides{1, iSize, iSize * jSize};

    serializationUnittestSerializer_->register_field("FortranField", typeID, dims, metaInfo);

    // Write to disk using SerialBox serializer
    serializationUnittestSerializer_->write("FortranField", savepoint_, fortranField, strides);

    files_.push_back("SerializationUnittest_FortranField.dat");

    // Load from disk using dycore::Serialization
    IJKStorageInfoType metaData(iSize, jSize, kSize);
    IJKRealField gridToolsField(metaData, -1, "GridToolsField");

    serialization_->load_gt("FortranField", gridToolsField, savepoint_);

    auto view = make_host_view(gridToolsField);
    // Verify result
    for (int k = 0; k < kSize; ++k)
        for (int j = 0; j < jSize; ++j)
            for (int i = 0; i < iSize; ++i)
                ASSERT_REAL_EQ(view(i, j, k), fortranField[k * jSize * iSize + j * iSize + i]);
}

#endif
