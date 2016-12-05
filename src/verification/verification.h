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

/**
 * @defgroup DycoreUnittestVerificationLibrary Verification
 * @brief Verifcation infrastructure of the Dycore Unittest library.
 *
 * @ingroup DycoreUnittestLibrary
 */
#pragma once

#include <vector>
#include "../common.h"
#include "../core/type_erased_field.h"
#include "boundary_extent.h"
#include "error_metric.h"
#include "verification_result.h"

GT_VERIFICATION_NAMESPACE_BEGIN

/**
 * @brief Verify if an output field (produced by a stencil) and a refrence field (loaded from disk)
 * are equal within a given @ref ErrorMetric "error metric".
 *
 * @ingroup DycoreUnittestVerificationLibrary
 */
class verification {
  public:
    /**
     * @brief Copy constructor
     */
    verification(const verification &) = default;

    /**
     * @brief Move constructor
     */
    verification(verification &&) = default;

    /**
     * @brief Assignment operator
     */
    verification &operator=(const verification &) = default;

    /**
     * @brief Initialize the verifcation
     *
     * @param outputField       Output field produced by a @ref StencilObjects "stencil object".
     * @param refrenceField     Refrence field loaded from disk.
     * @param errorMetric       @ref ErrorMetric "Error metric" to use.
     * @param boundary          Indentation of the output field.
     */
    verification(type_erased_field_view outputField,
        type_erased_field_view refrenceField,
        std::shared_ptr< error_metric > errorMetric,
        boundary_extent boundary = boundary_extent())
        : outputField_(outputField), referenceField_(refrenceField), errorMetric_(errorMetric), boundary_(boundary) {}

    /**
     * @brief Verify that outputField is equal to refrenceField within the given error metric
     *
     * This function discards all previous recorded failures.
     *
     * @return VerificationResult
     */
    verification_result verify() noexcept;

    /**
     * @brief Return true if errors occurred
     */
    bool has_errors() const noexcept { return !failures_.empty(); }

    /**
     * @brief Converts to true iff no errors occured
     */
    operator bool() const noexcept { return !has_errors(); }

    /**
     * @brief Represent a failure (mismatch of the outputField in respect to the referenceField)
     *
     * @ingroup DycoreUnittestVerificationLibrary
     */
    struct failure {
        int i;       /**< i position in the outputField */
        int j;       /**< j position in the outputField */
        int k;       /**< k position in the outputField */
        Real outVal; /**< Value of the outputField */
        Real refVal; /**< Value of the refrenceField */
    };
    static_assert(std::is_pod< failure >::value, "Verifcation::Failure should be POD.");

    /**
     * @brief Get the vector of @ref Failure "failures".
     */
    const std::vector< failure > &failures() const noexcept { return failures_; }

    /**
     * @brief Get a view to the output-field
     */
    type_erased_field_view output_field() const noexcept { return outputField_; }

    /**
     * @brief Get a view to the reference-field
     */
    type_erased_field_view reference_field() const noexcept { return referenceField_; }

  private:
    type_erased_field_view outputField_;
    type_erased_field_view referenceField_;
    std::shared_ptr< error_metric > errorMetric_;
    boundary_extent boundary_;

    std::vector< failure > failures_;
};

GT_VERIFICATION_NAMESPACE_END
