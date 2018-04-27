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
#include "../core/error.h"
#include "../core/logger.h"
#include "../core/serialization.h"
#include "../core/type_erased_field.h"
#include "../verification_exception.h"
#include "boundary_extent.h"
#include "error_metric_interface.h"
#include "verification.h"
#include "verification_reporter.h"
#include "verification_result.h"
#include "verification_specification.h"
#include <vector>

namespace gt_verification {

    namespace internal {

        struct savepoint_pair {
            template < class SavePointType >
            savepoint_pair(SavePointType &&_input, SavePointType &&_output)
                : input(std::forward< SavePointType >(_input)), output(std::forward< SavePointType >(_output)) {}

            ser::savepoint input;
            ser::savepoint output;
        };

        template < typename T >
        class input_field {
          public:
            input_field(std::string name, gt_verification::type_erased_field_view< T > field_view, bool also_previous)
                : name_(name), field_view_(field_view), also_previous_(also_previous) {}
            std::string name() const noexcept { return name_; }
            gt_verification::type_erased_field_view< T > field_view() const noexcept { return field_view_; }
            bool also_previous() const noexcept { return also_previous_; }

          private:
            const std::string name_;
            const gt_verification::type_erased_field_view< T > field_view_;
            const bool also_previous_;
        };
    }

    /**
     * @brief A collection of serialized fields
     *
     * @ingroup DycoreUnittestVerificationLibrary
     */
    template < typename T >
    class field_collection {
      public:
        field_collection(verification_specification verificationSpecification)
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
        void attach_reference_serializer(std::shared_ptr< ser::serializer > serializer,
            const std::string &inSavepointName,
            const std::string &outSavepointName) {
            referenceSerializer_ = serializer;

            const std::vector< ser::savepoint > &savepoints = referenceSerializer_->savepoints();
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
                        iterations_.push_back(internal::savepoint_pair(*rit, *it));
                }
            }
        }

        /**
         * @brief Attach an error serializer to the collection which will be used to serialize the
         * error result to disk [not implemented]
         *
         * @param serializer  The serializer which will be used to serialization
         *                    (in SerializerOpenModeWrite)
         */
        void attach_error_serializer(std::shared_ptr< ser::serializer > serializer) { errorSerializer_ = serializer; }

        /**
         * @brief Register an input field which will be filled during the loadIteration() function
         *
         * @param fieldname The name of the field as registered in the reference serializer
         * @param field     The field that has to be filled with data from disk
         */
        template < typename FieldType >
        void register_input_field(const std::string &fieldname, FieldType field, bool also_previous = false) noexcept {
        	field.sync();
            inputFields_.push_back(
                internal::input_field< T >{fieldname, type_erased_field_view< T >(field), also_previous});
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
        void register_output_and_reference_field(
            const std::string &fieldname, FieldType field, boundary_extent boundary = boundary_extent()) noexcept {
        	field.sync();
            boundaries_.push_back(boundary);
            outputFields_.push_back(std::make_pair(fieldname, type_erased_field_view< T >(field)));

            // Construct new field holding the reference data
            referenceFields_.push_back(std::make_pair(fieldname, type_erased_field< T >(field)));
        }

        /**
         * @brief Loads input values and reference values from disk into the input- and reference fields
         *
         * After this the computations and verification can take place.
         */
        void load_iteration(int iteration) {
            if (iteration >= (int)iterations_.size())
                error::fatal(boost::format("invalid access of iteration '%i' (there are only %i iterations)") %
                             iteration % iterations_.size());

            serialization serialization(referenceSerializer_);

            auto inputSavepoint = iterations_[iteration].input;
            auto refSavepoint = iterations_[iteration].output;

            try {
                // Load input fields
                VERIFICATION_LOG() << "Loading input savepoint '" << inputSavepoint << "'" << logger_action::endl;

                for (auto &inputFieldPair : inputFields_)
                    serialization.load(inputFieldPair.name(),
                        inputFieldPair.field_view(),
                        inputSavepoint,
                        inputFieldPair.also_previous());

                // Load reference fields
                VERIFICATION_LOG() << "Loading reference savepoint '" << refSavepoint << "'" << logger_action::endl;

                for (auto &refFieldPair : referenceFields_)
                    serialization.load(refFieldPair.first, refFieldPair.second.to_view(), refSavepoint);
            } catch (verification_exception &e) {
                error::fatal(e.what());
            }
        }

        /**
         * @brief Verifies all output sources and collects the result
         *
         * This function discards all previous recorded errors. To get a list of occured errors use
         * FieldCollection::reportFailures().
         *
         * @return VerificationResult
         */
        verification_result verify(const error_metric_interface< T > &error_metric) {
            verifications_.clear();

            verification_result totalResult(true, "\n");

            // Iterate over output fields and compare them to the reference fields
            for (std::size_t i = 0; i < outputFields_.size(); ++i) {
                verifications_.emplace_back(
                    outputFields_[i].second, referenceFields_[i].second.to_view(), boundaries_[i]);

                // Perform actual verification and merge results
                totalResult.merge(verifications_.back().verify(error_metric));
            }
            return totalResult;
        }

        /**
         * @brief Report failures depending on values set in VerificationReporter
         */
        void report_failures() const noexcept {
            verification_reporter verificationReporter(verificationSpecification_);
            for (const auto &verification : verifications_)
                if (!verification) {
                    verificationReporter.report(verification);
                }
        }

        /**
         * @brief Get the reference serializer
         */
        std::shared_ptr< ser::serializer > reference_serializer() const noexcept { return referenceSerializer_; }

        /**
         * @brief Get the error serializer
         */
        std::shared_ptr< ser::serializer > error_serializer() const noexcept { return errorSerializer_; }

        /**
         * @brief Get a const reference of the iterations
         */
        const std::vector< internal::savepoint_pair > &iterations() const noexcept { return iterations_; }

      private:
        std::string name_;
        std::shared_ptr< ser::serializer > referenceSerializer_;
        std::shared_ptr< ser::serializer > errorSerializer_;

        std::vector< internal::savepoint_pair > iterations_;

        std::vector< internal::input_field< T > > inputFields_;
        std::vector< std::pair< std::string, type_erased_field_view< T > > > outputFields_;
        std::vector< std::pair< std::string, type_erased_field< T > > > referenceFields_;
        std::vector< boundary_extent > boundaries_;

        verification_specification verificationSpecification_;
        std::vector< verification< T > > verifications_;
    };
}
