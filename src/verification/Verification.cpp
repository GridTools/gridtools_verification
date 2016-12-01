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
#include "Verification.h"
#include <boost/format.hpp>
#include <algorithm>
#include <numeric>
#include <vector>

GT_VERIFICATION_NAMESPACE_BEGIN

VerificationResult Verification::verify() noexcept {
#ifdef DYCORE_USE_GPU
    // Sync fields with Host
    outputField_.updateHost();
    referenceField_.updateHost();
#endif

    failures_.clear();

    std::string nameOut = outputField_.name();
    std::string nameRef = referenceField_.name();

    // Sizes *with* halo-boundaray
    const int iSizeOut = outputField_.iSize();
    const int jSizeOut = outputField_.jSize();
    const int kSizeOut = outputField_.kSize();

    const int iSizeRef = referenceField_.iSize();
    const int jSizeRef = referenceField_.jSize();
    const int kSizeRef = referenceField_.kSize();

    // Check dimensions
    if ((iSizeOut != iSizeRef) || (jSizeOut != jSizeRef) || (kSizeOut != kSizeRef))
        return VerificationResult(false,
            (boost::format("the output field '%s' has a diffrent size than the refrence "
                           "field '%s'.\n %-15s as: (%i, %i, %i)\n %-15s as: (%i, %i, %i)") %
                nameOut % nameRef % nameOut % iSizeOut % jSizeOut % kSizeOut % nameRef % iSizeRef % jSizeRef % kSizeRef)
                .str());

    // Verify fields
    for (int k = boundary_.kMinus(); k < (kSizeOut + boundary_.kPlus()); ++k)
        for (int j = boundary_.jMinus(); j < (jSizeOut + boundary_.jPlus()); ++j)
            for (int i = boundary_.iMinus(); i < (iSizeOut + boundary_.iPlus()); ++i)
                if (!errorMetric_->equal(outputField_(i, j, k), referenceField_(i, j, k)))
                    failures_.push_back(Failure{i, j, k, outputField_(i, j, k), referenceField_(i, j, k)});

    if (failures_.empty())
        return VerificationResult(true, "");
    else
        return VerificationResult(false,
            (boost::format("%5.3f %% of field entries of '%s' do not match (total of %i)") %
                (100 * Real(failures_.size()) / outputField_.size()) % nameOut % failures_.size())
                .str());
}

GT_VERIFICATION_NAMESPACE_END
