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
#include "FieldCollection.h"
#include "../VerificationException.h"
#include "../core/CommandLine.h"
#include "../core/Error.h"
#include "../core/Logger.h"
#include "../core/Serialization.h"
#include "VerificationReporter.h"
#include <algorithm>
#include <utility>

GT_VERIFICATION_NAMESPACE_BEGIN

void FieldCollection::attachReferenceSerializer(std::shared_ptr< ser::Serializer > serializer,
    const std::string &inSavepointName,
    const std::string &outSavepointName) {
    referenceSerializer_ = serializer;

    const std::vector< ser::Savepoint > &savepoints = referenceSerializer_->savepoints();
    iterations_.clear();

    for (auto it = savepoints.cbegin(), end = savepoints.cend(); it != end; ++it) {
        if (it->name() == outSavepointName) {
            // Output savepoint is found, now search input savepoint
            auto rit = std::reverse_iterator< decltype(it) >(it);
            while (rit != savepoints.rend()) {
                if (rit->name() == inSavepointName)
                    // Pair found, finish search
                    break;
                ++rit;
            }

            if (rit != savepoints.rend())
                iterations_.push_back(internal::SavepointPair(*rit, *it));
        }
    }
}

void FieldCollection::attachErrorSerializer(std::shared_ptr< ser::Serializer > serializer) {
    errorSerializer_ = serializer;
}

void FieldCollection::loadIteration(int iteration) {
    if (iteration >= (int)iterations_.size())
        Error::fatal(boost::format("invalid access of iteration '%i' (there are only %i iterations)") % iteration %
                     iterations_.size());

    Serialization serialization(referenceSerializer_);

    auto inputSavepoint = iterations_[iteration].input;
    auto refSavepoint = iterations_[iteration].output;

    try {
        // Load input fields
        VERIFICATION_LOG() << "Loading input savepoint '" << inputSavepoint << "'" << logger::endl;

        for (auto &inputFieldPair : inputFields_)
            serialization.load(inputFieldPair.first, inputFieldPair.second, inputSavepoint);

        // Load reference fields
        VERIFICATION_LOG() << "Loading reference savepoint '" << refSavepoint << "'" << logger::endl;

        for (auto &refFieldPair : referenceFields_)
            serialization.load(refFieldPair.first, refFieldPair.second.toView(), refSavepoint);
    } catch (VerificationException &e) {
        Error::fatal(e.what());
    }
}

VerificationResult FieldCollection::verify(std::shared_ptr< ErrorMetric > errorMetric) {
    verifications_.clear();

    VerificationResult totalResult(true, "\n");

    // Iterate over output fields and compare them to the reference fields
    for (std::size_t i = 0; i < outputFields_.size(); ++i) {
        verifications_.emplace_back(
            outputFields_[i].second, referenceFields_[i].second.toView(), errorMetric, boundaries_[i]);

        // Perform actual verification and merge results
        totalResult.merge(verifications_.back().verify());
    }

    return totalResult;
}

void FieldCollection::reportFailures() const noexcept {
    VerificationReporter verificationReporter(verificationSpecification_);
    for (const auto &verification : verifications_)
        if (!verification) {
            verificationReporter.report(verification);
        }
}

GT_VERIFICATION_NAMESPACE_END
