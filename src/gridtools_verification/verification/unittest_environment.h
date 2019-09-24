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

#include "field_collection.h"
#include "error_metric.h"
#include "../core.h"
#include <gtest/gtest.h>
#include <string>

namespace gt_verification {

    /**
     * @brief Provide access to the @ref DycoreObjects "dycore objecets" to all test-cases
     *
     * @ingroup DycoreUnittestVerificationLibrary
     */
    class unittest_environment : public testing::Environment,
                                 private boost::noncopyable /* singleton */
    {
      public:
        unittest_environment(command_line &cl, std::string data_name, std::string archive_type = "Binary")
            : cl_(cl), data_path_("./") {
            if (cl_.has("path"))
                data_path_ = cl_.as< std::string >("path");

            VERIFICATION_LOG() << "Using serializer data-path: '" << data_path_ << "'" << logger_action::endl;

            // Initialize the serializer
            reference_serializer_ =
                std::make_shared< ser::serializer >(ser::open_mode::Read, data_path_, data_name, archive_type);

            // Initialize error serializer
            error_serializer_ = std::make_shared< ser::serializer >(ser::open_mode::Write, ".", "Error");
        };

        static unittest_environment &get_instance();

        /**
         * @brief Returns the name of the current test as registered by GTest
         *
         * @return The test name in the form $testcasename.$testname is returned
         */
        std::string test_name() const noexcept;

        /**
         * @brief Print skipped tests (if any)
         */
        void print_skipped_tests() const noexcept;

        /**
         * @brief SetUp the global test environment (called by GTest)
         */
        //    virtual void SetUp() override;

        /**
         * @brief TearDown the global test environment (called by GTest)
         */
        virtual void TearDown() override;

        /**
         * @brief Get the reference serialbox serializer
         *
         * @see Serialization
         */
        std::shared_ptr< ser::serializer > reference_serializer() const noexcept { return reference_serializer_; }

        /**
         * @brief Get the error serialbox serializer
         *
         * @see Serialization
         */
        std::shared_ptr< ser::serializer > error_serializer() const noexcept { return error_serializer_; }

        /**
         * @brief Initializes and returns a collection for the tests
         *
         * This function must be called by the SetUp function of the tests in which the unittest data is
         * needed. It will create a collection and initialize it with the provided name or, in case the
         * name is not given, with the name of the unittest as returned by TestName.
         *
         * If no suitable savepoint pairs are found, this function will print a message integrated in
         * the GTest output to mark the test as skipped.
         */
        //    std::shared_ptr<field_collection> createfield_collection(std::string spname = "");
        template < typename T >
        field_collection< T > create_field_collection(std::string spname = "") {
            if (spname.empty())
                spname = test_name();

            VERIFICATION_LOG() << "Creating field collection for '" << spname << "'" << logger_action::endl;

            verification_specification verifSpec(cl_);
            field_collection< T > collection(verifSpec);
            collection.attach_reference_serializer(reference_serializer(), spname + "-in", spname + "-out");
            collection.attach_error_serializer(error_serializer());

            if (collection.iterations().size() == 0) {
                cprintf(color::YELLOW, "[   SKIP   ]");
                std::printf(" Skipping test because there is no reference data\n");
                skipped_.push_back(spname);
            }

            return collection;
        }

        void skip_test(std::string spname = "");

        /**
         * @brief Create a standard error metric
         *
         * @param rtol  The relative error tolerance (optional)
         * @param atol  The absolute error tolerance (optional)
         *
         * @see error_metric
         */
        template < typename T >
        error_metric< T > create_metric(T rtol = 1e-06, T atol = 1e-10) const noexcept {
            return error_metric< T >(rtol, atol);
        }

        /**
         * @brief Verifies a field collection
         *
         * In case of a failure a detailed error message will be printed as provided by
         * TestResult::printFailures()
         *
         * @return testing::AssertionSuccess() if no failures occured, testing::AssertionFailure()
         * otherwise
         */
        template < typename T >
        testing::AssertionResult verify_collection(
            field_collection< T > &fieldCollection, const error_metric_interface< T > &errorMetric) {
            verification_result result = fieldCollection.verify(errorMetric);
            if (!result.passed())
                fieldCollection.report_failures();

            return result.toAssertionResult();
        }

      protected:
        command_line &cl_;

        std::string data_path_;

        // Serializer objects
        std::shared_ptr< ser::serializer > reference_serializer_;
        std::shared_ptr< ser::serializer > error_serializer_;

        // List of skipped tests
        std::vector< std::string > skipped_;

      public:
        static unittest_environment *instance_;
    };
}
