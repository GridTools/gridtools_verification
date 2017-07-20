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
#include <gridtools.hpp>
#include <storage/storage-facility.hpp>

using namespace gt_verification;


#ifdef HAS_GRIDTOOLS

using Real = double;
namespace internal2
{


#ifdef DYCORE_USE_GPU
using StorageTraitsType = gridtools::storage_traits<gridtools::enumtype::Cuda>;
using IJKLayoutType = gridtools::layout_map<2, 1, 0>; // stride 1 on i
#else
using StorageTraitsType = gridtools::storage_traits<gridtools::enumtype::Host>;
using IJKLayoutType = gridtools::layout_map<0, 1, 2>; // stride 1 on k
#endif

const int iMinusHaloSize = 3;
const int iPlusHaloSize = 3;
const int jMinusHaloSize = 3;
const int jPlusHaloSize = 3;

using IJHaloType = gridtools::halo<iMinusHaloSize, jMinusHaloSize, 0>;
} // end namespace internal

using IJKMetaStorageType = internal2::StorageTraitsType::meta_storage_type<0,
                                                                          internal2::IJKLayoutType,
                                                                          internal2::IJHaloType>;
namespace internal2
{
using IJKRealStorageType = StorageTraitsType::storage_type<Real, IJKMetaStorageType>;
using IJKRealTemporaryStorageType
    = StorageTraitsType::temporary_storage_type<Real, IJKMetaStorageType>;
} // end namespace internal

using IJKRealField = internal2::IJKRealStorageType;


/**
 * @brief Serialization unittest
 */
 class SerializationUnittest : public ::testing::Test {
  public:
    /**
     * Fill GridTools field with uniqiue values
     */
    template < class FieldType >
    static void fillUniqueValues(FieldType &field) {
        for (unsigned i = 0; i < iSize; ++i)
            for (unsigned j = 0; j < jSize; ++j)
                for (unsigned k = 0; k < kSize; ++k)
                    field(i, j, k) = i + j + k;
#ifdef DYCORE_USE_GPU
        field.h2d_update();
#endif
    }

  protected:
    /// SerialBox objects
    std::shared_ptr< ser::Serializer > serializationUnittestSerializer_;
    ser::Savepoint savepoint_;

    /// Serialization, contains the load and write methods
    std::shared_ptr< serialization > serialization_;

    /// Record the files we created (to cleanup in the end)
    std::vector< std::string > files_;

    /// Sizes of the fields
    static constexpr int iSize = 33;
    static constexpr int jSize = 22;
    static constexpr int kSize = 80;

  protected:
    virtual void SetUp() override {
        serializationUnittestSerializer_ = std::make_shared< ser::Serializer >();
        serializationUnittestSerializer_->Init(".", "SerializationUnittest", ser::SerializerOpenModeWrite);
        savepoint_.Init("TestPoint");

        serialization_ = std::make_shared< serialization >(serializationUnittestSerializer_);
        files_.clear();
        files_.push_back("SerializationUnittest.json");
    }

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
    IJKMetaStorageType metaData(iSize, jSize, kSize);

    IJKRealField gridToolsField1(metaData, -1, "GridToolsField1");
    fillUniqueValues(gridToolsField1);

    // Write to disk
    serialization_->write<Real>("GridToolsField", gridToolsField1, savepoint_);
    files_.push_back("SerializationUnittest_GridToolsField.dat");

    // Load from disk
    IJKRealField gridToolsField2(metaData, -1, "GridToolsField2");
    serialization_->load("GridToolsField", gridToolsField2, savepoint_);

    // Verify result
    for (int i = 0; i < iSize; ++i)
        for (int j = 0; j < jSize; ++j)
            for (int k = 0; k < kSize; ++k)
                ASSERT_REAL_EQ(gridToolsField2(i, j, k), gridToolsField1(i, j, k));
}

/**
 * Serialize a "fake" fortran field to disk and load it into a GridTools field
 */
 TEST_F(SerializationUnittest, FortranToGridTools) {
    // Fortran field (Column-major)
    std::unique_ptr< Real[] > fortranField(new Real[iSize * jSize * kSize]);

    for (int k = 0; k < kSize; ++k)
        for (int j = 0; j < jSize; ++j)
            for (int i = 0; i < iSize; ++i)
                fortranField[k * jSize * iSize + j * iSize + i] = i + j + k;

    // Register field using SerialBox serializer
    const int bytesPerElement = sizeof(Real);
    serializationUnittestSerializer_->RegisterField("FortranField",
        ser::type_name< Real >(),
        bytesPerElement,
        iSize,
        jSize,
        kSize,
        1,
        internal2::iMinusHaloSize,
        internal2::iPlusHaloSize,
        internal2::jMinusHaloSize,
        internal2::jPlusHaloSize,
        0,
        0,
        0,
        0);

    // Write to disk using SerialBox serializer
    serializationUnittestSerializer_->WriteField("FortranField",
        savepoint_,
        static_cast< void * >(fortranField.get()),
        1 * bytesPerElement,
        iSize * bytesPerElement,
        iSize * jSize * bytesPerElement,
        0);

    files_.push_back("SerializationUnittest_FortranField.dat");

    // Load from disk using dycore::Serialization
    IJKMetaStorageType metaData(iSize, jSize, kSize);
    IJKRealField gridToolsField(metaData, -1, "GridToolsField");

    serialization_->load("FortranField", gridToolsField, savepoint_);

    // Verify result
    for (int k = 0; k < kSize; ++k)
        for (int j = 0; j < jSize; ++j)
            for (int i = 0; i < iSize; ++i)
                ASSERT_REAL_EQ(gridToolsField(i, j, k), fortranField[k * jSize * iSize + j * iSize + i]);
}

#endif

