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

GT_VERIFICATION_NAMESPACE_BEGIN

class boundary_extent {
  public:
    boundary_extent(boundary_extent &&) = default;
    boundary_extent(const boundary_extent &) = default;
    boundary_extent &operator=(const boundary_extent &) = default;

    boundary_extent(int iMinus = 0, int iPlus = 0, int jMinus = 0, int jPlus = 0, int kMinus = 0, int kPlus = 0) {
        iMinus_ = iMinus;
        iPlus_ = iPlus;
        jMinus_ = jMinus;
        jPlus_ = jPlus;
        kMinus_ = kMinus;
        kPlus_ = kPlus;
    }

    /**
     * @brief Indendation on the top
     */
    int i_minus() const noexcept { return iMinus_; }

    /**
     * @brief Indendation on the bottom
     */
    int i_plus() const noexcept { return iPlus_; }

    /**
     * @brief Indendation on the left
     */
    int j_minus() const noexcept { return jMinus_; }

    /**
     * @brief Indendation on the right
     */
    int j_plus() const noexcept { return jPlus_; }

    /**
     * @brief Indendation at the top in k-direction
     */
    int k_minus() const noexcept { return kMinus_; }

    /**
     * @brief Indendation at the bottom in k-direction
     */
    int k_plus() const noexcept { return kPlus_; }

  private:
    int iMinus_, iPlus_;
    int jMinus_, jPlus_;
    int kMinus_, kPlus_;
};

GT_VERIFICATION_NAMESPACE_END