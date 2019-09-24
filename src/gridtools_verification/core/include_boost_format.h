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

// In boost 1.58 there is a problem with BOOST_NO_CXX11_DECLTYPE when including boost/format.hpp
#ifdef BOOST_NO_CXX11_DECLTYPE
#undef BOOST_NO_CXX11_DECLTYPE
#define BOOST_NO_CXX11_DECLTYPE_WAS_DEFINED
#endif
#include <boost/format.hpp>
#ifdef BOOST_NO_CXX11_DECLTYPE_WAS_DEFINED
#undef BOOST_NO_CXX11_DECLTYPE_WAS_DEFINED
#define BOOST_NO_CXX11_DECLTYPE
#endif
