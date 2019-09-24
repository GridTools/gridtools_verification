/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "verification_reporter.h"

namespace gt_verification {
    template void verification_reporter::report< float >(const verification< float > &Verification) const noexcept;
    template void verification_reporter::report< double >(const verification< double > &Verification) const noexcept;
}
