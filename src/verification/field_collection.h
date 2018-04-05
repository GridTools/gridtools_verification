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

#include "verification_specification.h"
#include <vector>
#include "../common.h"
#include "../core/type_erased_field.h"
#include "../core/serialization.h"
#include "../core/error.h"
#include "../core/logger.h"
#include "../verification_exception.h"
#include "verification_reporter.h"
#include "error_metric_interface.h"
#include "boundary_extent.h"
#include "verification.h"
#include "verification_result.h"

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
                : name_(name), field_view_(field_view) {}
            std::string name() const noexcept { return name_; }
            gt_verification::type_erased_field_view< T > field_view() const noexcept { return field_view_; }

          private:
            const std::string name_;
            const gt_verification::type_erased_field_view< T > field_view_;
        };
    }

    // TODO a clean solution for the iterator would iterate over something like this.
    //    class verification_savepoint {
    //      public:
    //        void load_input(...)
    //        {
    //        }
    //        verification_result verify(..)
    //        {
    //        }
    //      private:
    //        ...
    //    };

    /**
     * @brief A collection of serialized fields
     *
     * @ingroup DycoreUnittestVerificationLibrary
     */
    template < typename T >
    class field_collection {
      public:
        field_collection(verification_specification verificationSpecification) : reporter_(verificationSpecification){};

        field_collection(const field_collection &) = delete;
        field_collection &operator=(const field_collection &) = delete;
        field_collection(field_collection &&) = default;
        field_collection &operator=(field_collection &&) = default;

        struct collection_iterator {
            size_t pos_;
            // TODO this is a bit hacky, but I think good enough for now,
            // the clean solution would be to iterate over a list of s
            field_collection< T > &collection_;

            collection_iterator(size_t pos, field_collection< T > &collection) : pos_(pos), collection_(collection) {}

            field_collection< T > &operator*() {
                collection_.active_iteration_ = pos_; // TODO
                return collection_;
            };

            collection_iterator &operator++() {
                pos_++;
                return *this;
            }

            collection_iterator operator++(int) {
                collection_iterator tmp(*this);
                operator++();
                return tmp;
            }

            bool operator==(const collection_iterator &other) const { return pos_ == other.pos_; }
            bool operator!=(const collection_iterator &other) const { return !operator==(other); }
        };

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
        GV_DEPRECATED_REASON(void register_input_field(
                                 const std::string &fieldname, FieldType &field, bool also_previous = false) noexcept,
            "Use the c++11 iterator with iterator.load_input(...).") {
            inputFields_.push_back(
                internal::input_field< T >{fieldname, type_erased_field_view< T >(field), also_previous});
        }

        template < typename FieldType >
        void load_input(const std::string &fieldname, FieldType &field) {
            serialization serialization(referenceSerializer_);
            auto inputSavepoint = iterations_[active_iteration_].input;
            type_erased_field_view< T > f(field);
            serialization.load(fieldname, f, inputSavepoint);
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
        GV_DEPRECATED_REASON(
            void register_output_and_reference_field(
                const std::string &fieldname, FieldType &field, boundary_extent boundary = boundary_extent()) noexcept,
            "Use the c++11 iterator with iterator.verify_output(...).") {
            boundaries_.push_back(boundary);
            outputFields_.push_back(std::make_pair(fieldname, type_erased_field_view< T >(field)));

            // Construct new field holding the reference data
            referenceFields_.push_back(std::make_pair(fieldname, type_erased_field< T >(field)));
        }

        /**
         * @brief verify an output field
         *
         * This is a new interface where you don't register the output field but call the verify with the field.
         */
        template < typename FieldType >
        verification_result add_output(const std::string &fieldname,
            FieldType &field,
            const error_metric_interface< T > &error_metric,
            boundary_extent boundary = boundary_extent()) noexcept {

            auto refSavepoint = iterations_[active_iteration_].output;

            type_erased_field< T > reference_field(field);

            serialization serialization(referenceSerializer_);
            serialization.load(fieldname, reference_field.to_view(), refSavepoint);

            verifications_.emplace_back(type_erased_field_view< T >(field), reference_field.to_view(), boundary);
            result_.merge(verifications_.back().verify(error_metric));

            return result_;
        }

        /**
         * @brief Loads input values and reference values from disk into the input- and reference fields
         *
         * After this the computations and verification can take place.
         */
        GV_DEPRECATED_REASON(void load_iteration(int iteration),
            "Use the c++11 iterator with iterator.load_input(...) and iterator.verify_output(...).") {
            if (iteration >= (int)iterations_.size())
                error::fatal(boost::format("invalid access of iteration '%i' (there are only %i iterations)") %
                             iteration % iterations_.size());

            serialization serialization(referenceSerializer_);

            active_iteration_ = iteration;

            auto inputSavepoint = iterations_[iteration].input;
            auto refSavepoint = iterations_[iteration].output;

            try {
                // Load input fields
                VERIFICATION_LOG() << "Loading input savepoint '" << inputSavepoint << "'" << logger_action::endl;

                for (auto &inputFieldPair : inputFields_)
                    serialization.load(inputFieldPair.name(), inputFieldPair.field_view(), inputSavepoint);

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
            for (const auto &verification : verifications_)
                if (!verification) {
                    reporter_.report(verification);
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

        collection_iterator begin() { return collection_iterator(0, *this); }
        collection_iterator end() { return collection_iterator(iterations_.size(), *this); }

        /**
         * @brief reports errors and resets the error state
         */
        verification_result verify_collection() {
            if (!result_.passed())
                report_failures();

            verification_result tmp = result_;
            verifications_.clear();
            result_.clear();
            return tmp;
        }

        int active_iteration_;

      private:
        std::string name_;
        std::shared_ptr< ser::serializer > referenceSerializer_;
        std::shared_ptr< ser::serializer > errorSerializer_;

        std::vector< internal::savepoint_pair > iterations_;

        std::vector< internal::input_field< T > > inputFields_;
        std::vector< std::pair< std::string, type_erased_field_view< T > > > outputFields_;
        std::vector< std::pair< std::string, type_erased_field< T > > > referenceFields_;
        std::vector< boundary_extent > boundaries_;

        verification_reporter reporter_;
        std::vector< verification< T > > verifications_;
        verification_result result_;
    };
}
