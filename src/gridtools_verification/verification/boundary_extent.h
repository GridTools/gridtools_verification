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

#include "../common.h"

namespace gt_verification {

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
}
