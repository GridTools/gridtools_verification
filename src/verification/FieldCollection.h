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

#include "VerificationSpecification.h"
#include <vector>
#include "../Common.h"
#include "../core/TypeErasedField.h"
#include "../core/Serialization.h"
#include "ErrorMetric.h"
#include "BoundaryExtent.h"
#include "Verification.h"
#include "VerificationResult.h"

GT_VERIFICATION_NAMESPACE_BEGIN

namespace internal {

    struct SavepointPair {
        template < class SavePointType >
        SavepointPair(SavePointType &&_input, SavePointType &&_output)
            : input(std::forward< SavePointType >(_input)), output(std::forward< SavePointType >(_output)) {}

        ser::Savepoint input;
        ser::Savepoint output;
    };
}

/**
 * @brief A collection of serialized fields
 *
 * @ingroup DycoreUnittestVerificationLibrary
 */
class FieldCollection {
  public:
    FieldCollection(VerificationSpecification verificationSpecification)
        : verificationSpecification_(verificationSpecification){};

    /**
     * @brief Attach a reference serializer to the collection.
     *
     * After this, input and reference sources can be registred within the collection via
     * FieldCollection::registerInputField and FieldCollection::registerOutputAndRefrenceField.
     * The serializer must be already open in read mode. The serializer is searched for all
     * savepoints whose name correponds to the @c outSavepointName parameter and for all
     * corresponding input savepoints.
     *
     * @param serializer        The serializer that loads the reference data
     * @param outSavepointName  The name of the savepoint where the reference values are stored
     * @param inSavepointName   The name of the savepoint where the input values are stored
     */
    void attachReferenceSerializer(std::shared_ptr< ser::Serializer > serializer,
        const std::string &inSavepointName,
        const std::string &outSavepointName);

    /**
     * @brief Attach an error serializer to the collection which will be used to serialize the
     * error result to disk [not implemented]
     *
     * @param serializer  The serializer which will be used to serialization
     *                    (in SerializerOpenModeWrite)
     */
    void attachErrorSerializer(std::shared_ptr< ser::Serializer > serializer);

    /**
     * @brief Register an input field which will be filled during the loadIteration() function
     *
     * @param fieldname The name of the field as registered in the reference serializer
     * @param field     The field that has to be filled with data from disk
     */
    template < typename FieldType >
    void registerInputField(const std::string &fieldname, FieldType &field) noexcept {
        inputFields_.push_back(std::make_pair(fieldname, TypeErasedFieldView(field)));
    }

    /**
     * @brief Register an output field for verification
     *
     * The registered output field will be validated against a newly allocate reference field which
     * is going to be loaded from disk (using the reference serializer).
     *
     * @param fieldname The name of the field as serialized
     * @param field     The field that has to be checked
     * @param metric    The metric which is used to check the field
     */
    template < typename FieldType >
    void registerOutputAndReferenceField(
        const std::string &fieldname, FieldType &field, BoundaryExtent boundary = BoundaryExtent()) noexcept {
        boundaries_.push_back(boundary);
        outputFields_.push_back(std::make_pair(fieldname, TypeErasedFieldView(field)));

        // Construct new field holding the reference data
        referenceFields_.push_back(std::make_pair(fieldname, TypeErasedField(field)));
    }

    /**
     * @brief Loads input values and reference values from disk into the input- and reference fields
     *
     * After this the computations and verification can take place.
     */
    void loadIteration(int iteration);

    /**
     * @brief Verifies all output sources and collects the result
     *
     * This function discards all previous recorded errors. To get a list of occured errors use
     * FieldCollection::reportFailures().
     *
     * @return VerificationResult
     */
    VerificationResult verify(std::shared_ptr< ErrorMetric > errorMetric);

    /**
     * @brief Report failures depending on values set in VerificationReporter
     */
    void reportFailures() const noexcept;

    /**
     * @brief Get the reference serializer
     */
    std::shared_ptr< ser::Serializer > referenceSerializer() const noexcept { return referenceSerializer_; }

    /**
     * @brief Get the error serializer
     */
    std::shared_ptr< ser::Serializer > errorSerializer() const noexcept { return errorSerializer_; }

    /**
     * @brief Get a const reference of the iterations
     */
    const std::vector< internal::SavepointPair > &iterations() const noexcept { return iterations_; }

  private:
    std::string name_;
    std::shared_ptr< ser::Serializer > referenceSerializer_;
    std::shared_ptr< ser::Serializer > errorSerializer_;

    std::vector< internal::SavepointPair > iterations_;

    std::vector< std::pair< std::string, TypeErasedFieldView > > inputFields_;
    std::vector< std::pair< std::string, TypeErasedFieldView > > outputFields_;
    std::vector< std::pair< std::string, TypeErasedField > > referenceFields_;
    std::vector< BoundaryExtent > boundaries_;

    VerificationSpecification verificationSpecification_;
    std::vector< Verification > verifications_;
};

GT_VERIFICATION_NAMESPACE_END
