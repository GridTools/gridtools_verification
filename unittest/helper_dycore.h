/**
 *
 */

#pragma once

#ifdef HAS_GRIDTOOLS

#include <gridtools/storage/storage-facility.hpp>

using Real = double;
const int iMinusHaloSize = 3;
const int iPlusHaloSize = 3;
const int jMinusHaloSize = 3;
const int jPlusHaloSize = 3;

namespace {

#ifdef DYCORE_USE_GPU
    using StorageTraitsType = gridtools::storage_traits< gridtools::platform::cuda >;
#else
    using StorageTraitsType = gridtools::storage_traits< gridtools::platform::x86 >;
#endif

    using IJHaloType = gridtools::halo< iMinusHaloSize, jMinusHaloSize, 0 >;
} // namespace

using IJKStorageInfoType = ::StorageTraitsType::storage_info_t< 0, 3, ::IJHaloType >;
using IJKRealField = ::StorageTraitsType::data_store_t< Real, IJKStorageInfoType >;

#endif
