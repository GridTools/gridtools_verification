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

#include <vector>
#include "../common.h"

GT_VERIFICATION_NAMESPACE_BEGIN

/**
 * @brief Flush the cache for OpenMP codes
 *
 * @ingroup DycoreUnittestBenchmarkLibrary
 */
class cache_flusher {
  public:
    /**
     * @brief Initialize with (n >= cache size) to flush all level of caches
     *
     * @param n     Last level cache size in bytes (e.g 8 MB == 8388608)
     */
    cache_flusher(int n = 8388608) {
        assert(n > 2);

        // We are generous and allocate 3 * n bytes
        n_ = n / 8;
        a_.resize(n_);
        b_.resize(n_);
        c_.resize(n_);
    }

    /**
     * @brief Flush the cache
     */
    void flush() noexcept {
        double *a = &a_[0];
        double *b = &b_[0];
        double *c = &c_[0];
        int i;

#pragma omp parallel for private(i)
        for (i = 0; i < n_; i++)
            a[i] = b[i] * c[i];
    };

  private:
    std::vector< double > a_;
    std::vector< double > b_;
    std::vector< double > c_;
    int n_;
};

GT_VERIFICATION_NAMESPACE_END
