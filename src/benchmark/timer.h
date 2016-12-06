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
#pragma once

#include "../common.h"

#include <stencil-composition/backend_cuda/timer_cuda.hpp>
#include <stencil-composition/backend_host/timer_host.hpp>

GT_VERIFICATION_NAMESPACE_BEGIN

/**
 * @defgroup Timer
 * @brief Wrap @c gridtools timers.
 *
 * @b NOTE: OpenMP must be enabled for measuring on the host.
 *
 * @ingroup DycoreUnittestBenchmarkLibrary
 * @{
 */

/**
 * @typedef Timer
 * @brief @c gridtools timer to measure on the host or device
 *
 * @code{.cpp}
 * Timer t("myTimer");
 * t.start();
 * // ... Do some work
 * t.pause();
 * @endcode
 * @ingroup Timer
 * @{
 */

namespace {
    template < gridtools::enumtype::platform Platform >
    struct select_timer {
        using timer = gridtools::timer_host;
    };

    template <>
    struct select_timer< gridtools::enumtype::Cuda > {
        using timer = gridtools::timer_cuda;
    };
}

template < gridtools::enumtype::platform Platform >
using timer = typename select_timet< Platform >::type;

/** @} */

/** @} */

GT_VERIFICATION_NAMESPACE_END
