/**
 *
 */

#pragma once

#ifdef HAS_GRIDTOOLS

#include <storage/storage-facility.hpp>

using Real = double;
namespace internal2 {

#ifdef DYCORE_USE_GPU
    using StorageTraitsType = gridtools::storage_traits< gridtools::enumtype::Cuda >;
    using IJKLayoutType = gridtools::layout_map< 2, 1, 0 >; // stride 1 on i
#else
    using StorageTraitsType = gridtools::storage_traits< gridtools::enumtype::Host >;
    using IJKLayoutType = gridtools::layout_map< 0, 1, 2 >; // stride 1 on k
#endif

    const int iMinusHaloSize = 3;
    const int iPlusHaloSize = 3;
    const int jMinusHaloSize = 3;
    const int jPlusHaloSize = 3;

    using IJHaloType = gridtools::halo< iMinusHaloSize, jMinusHaloSize, 0 >;
} // end namespace internal

using IJKMetaStorageType =
    internal2::StorageTraitsType::meta_storage_type< 0, internal2::IJKLayoutType, internal2::IJHaloType >;
namespace internal2 {
    using IJKRealStorageType = StorageTraitsType::storage_type< Real, IJKMetaStorageType >;
    using IJKRealTemporaryStorageType = StorageTraitsType::temporary_storage_type< Real, IJKMetaStorageType >;
} // end namespace internal

using IJKRealField = internal2::IJKRealStorageType;

#endif
