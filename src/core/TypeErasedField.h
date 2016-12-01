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

#include <type_traits>
#include <utility>
#include <memory>
#include <boost/mpl/bool.hpp>
#include "Common.h"

GT_VERIFICATION_NAMESPACE_BEGIN

// FIXME remove dependency on a compile-time flag
#ifndef FLOAT_PRECISION
#define FLOAT_PRECISION 8
#endif

#if FLOAT_PRECISION == 4
using Real = float;
#elif FLOAT_PRECISION == 8
using Real = double;
#else
#error float precision not properly set (4 or 8 bytes expected)
#endif

namespace internal {

    /**
     * Type erased interface for GridTools fields with value_type dycore::Real
     */
    class TypeErasedFieldInterface : private boost::noncopyable {
      public:
        virtual ~TypeErasedFieldInterface() {}

        /**
         * Access the field at position (i, j, k) and return a const refrence of the held value
         */
        virtual const Real &access(int i, int j, int k) const noexcept = 0;

        /**
         * Access the field at position (i, j, k) and return a refrence of the held value
         */
        virtual Real &access(int i, int j, int k) noexcept = 0;

        /**
         * Pointer to the storage
         */
        virtual Real *data() noexcept = 0;

        /**
         * Const pointer to the storage
         */
        virtual const Real *data() const noexcept = 0;

        /**
         * Get the name of the field
         */
        virtual const char *name() const noexcept = 0;

        /**
         * First dimension (i-direction) excluding halo-boundaries
         */
        virtual int iSize() const noexcept = 0;

        /**
         * First dimension (i-direction) including halo-boundaries
         *
         * For non-padded fields this is equal to iSizePadded()
         */
        virtual int iSizeNoHalo() const noexcept = 0;

        /**
         * First dimension (i-direction) including potential padding
         */
        virtual int iSizePadded() const noexcept = 0;

        /**
         * Second dimension (j-direction) including halo-boundaries
         */
        virtual int jSize() const noexcept = 0;

        /**
         * First dimension (i-direction) including halo-boundaries
         *
         * For non-padded fields this is equal to jSizePadded()
         */
        virtual int jSizeNoHalo() const noexcept = 0;

        /**
         * First dimension (i-direction) including potential padding
         */
        virtual int jSizePadded() const noexcept = 0;

        /**
         * Third dimension (k-direction)
         */
        virtual int kSize() const noexcept = 0;

        /**
         * Third dimension (k-direction) including potential padding
         */
        virtual int kSizePadded() const noexcept = 0;

        /**
         * Stride in i-direction
         */
        virtual int iStride() const noexcept = 0;

        /**
         * Stride in j-direction
         */
        virtual int jStride() const noexcept = 0;

        /**
         * Stride in k-direction
         */
        virtual int kStride() const noexcept = 0;

        /**
         * Update device pointer (calls h2d_update)
         */
        virtual void updateDevice() noexcept = 0;

        /**
         * Update host pointer (calls d2h_update)
         */
        virtual void updateHost() noexcept = 0;
    };

    template < class FieldType >
    class TypeErasedFieldViewBase : public TypeErasedFieldInterface {
      public:
        TypeErasedFieldViewBase(FieldType &field) : field_(field) {}

        virtual const Real &access(int i, int j, int k) const noexcept override { return field_(i, j, k); }

        virtual Real &access(int i, int j, int k) noexcept override { return field_(i, j, k); }

        virtual Real *data() noexcept override { return &field_(0, 0, 0); }

        virtual const Real *data() const noexcept override { return &field_(0, 0, 0); }

        virtual const char *name() const noexcept override { return field_.get_name(); }

        virtual int iSize() const noexcept override { return field_.meta_data().template unaligned_dim< 0 >(); }

        virtual int iSizeNoHalo() const noexcept override {
            // TODO: This is a temporary workaround to query the halo padding
            return (field_.meta_data().template unaligned_dim< 0 >() -
                    FieldType::storage_info_type::halo_t::template get< 0 >());
        }

        virtual int iSizePadded() const noexcept override { return field_.meta_data().template dim< 0 >(); }

        virtual int jSize() const noexcept override { return field_.meta_data().template unaligned_dim< 1 >(); }

        virtual int jSizeNoHalo() const noexcept override {
            // TODO: This is a temporary workaround to query the halo padding
            return (field_.meta_data().template unaligned_dim< 1 >() -
                    FieldType::storage_info_type::halo_t::template get< 1 >());
        }

        virtual int jSizePadded() const noexcept override { return field_.meta_data().template dim< 1 >(); }

        virtual int kSize() const noexcept override { return field_.meta_data().template unaligned_dim< 2 >(); }

        virtual int kSizePadded() const noexcept override { return field_.meta_data().template dim< 2 >(); }

        virtual int iStride() const noexcept override { return field_.meta_data().template strides< 0 >(); }

        virtual int jStride() const noexcept override { return field_.meta_data().template strides< 1 >(); }

        virtual int kStride() const noexcept override { return field_.meta_data().template strides< 2 >(); }

        virtual void updateDevice() noexcept override {
#ifdef DYCORE_USE_GPU
            field_.h2d_update();
#endif
        }

        virtual void updateHost() noexcept override {
#ifdef DYCORE_USE_GPU
            field_.set_on_device(); // TODO enforce copy: bug in expandable parameters does not properly
                                    // set flag
            field_.d2h_update();
#endif
        }

      private:
        FieldType &field_;
    };

    template < class FieldType >
    class TypeErasedFieldBase : public TypeErasedFieldInterface {
      public:
        TypeErasedFieldBase(const FieldType &field)
            : metaData_(field.meta_data().template unaligned_dim< 0 >(),
                  field.meta_data().template unaligned_dim< 1 >(),
                  field.meta_data().template unaligned_dim< 2 >()),
              field_(metaData_, -1, field.get_name()) {

#ifdef DYCORE_USE_GPU
            // Update field on host
            field_.d2h_update();
#endif

            // Copy field
            int iSize = this->iSize();
            int jSize = this->jSize();
            int kSize = this->kSize();
            for (int i = 0; i < iSize; ++i)
                for (int j = 0; j < jSize; ++j)
                    for (int k = 0; k < kSize; ++k)
                        field_(i, j, k) = field(i, j, k);
        }

        virtual const Real &access(int i, int j, int k) const noexcept override { return field_(i, j, k); }

        virtual Real &access(int i, int j, int k) noexcept override { return field_(i, j, k); }

        virtual Real *data() noexcept override { return &field_(0, 0, 0); }

        virtual const Real *data() const noexcept override { return &field_(0, 0, 0); }

        virtual const char *name() const noexcept override { return field_.get_name(); }

        virtual int iSize() const noexcept override { return field_.meta_data().template unaligned_dim< 0 >(); }

        virtual int iSizeNoHalo() const noexcept override {
            // TODO: This is a temporary workaround to query the halo padding
            return (field_.meta_data().template unaligned_dim< 0 >() -
                    FieldType::storage_info_type::halo_t::template get< 0 >());
        }

        virtual int iSizePadded() const noexcept override { return field_.meta_data().template dim< 0 >(); }

        virtual int jSize() const noexcept override { return field_.meta_data().template unaligned_dim< 1 >(); }

        virtual int jSizeNoHalo() const noexcept override {
            // TODO: This is a temporary workaround to query the halo padding
            return (field_.meta_data().template unaligned_dim< 1 >() -
                    FieldType::storage_info_type::halo_t::template get< 1 >());
        }

        virtual int jSizePadded() const noexcept override { return field_.meta_data().template dim< 1 >(); }

        virtual int kSize() const noexcept override { return field_.meta_data().template unaligned_dim< 2 >(); }

        virtual int kSizePadded() const noexcept override { return field_.meta_data().template dim< 2 >(); }

        virtual int iStride() const noexcept override { return field_.meta_data().template strides< 0 >(); }

        virtual int jStride() const noexcept override { return field_.meta_data().template strides< 1 >(); }

        virtual int kStride() const noexcept override { return field_.meta_data().template strides< 2 >(); }

        virtual void updateDevice() noexcept override {
#ifdef DYCORE_USE_GPU
            field_.h2d_update();
#endif
        }

        virtual void updateHost() noexcept override {
#ifdef DYCORE_USE_GPU
            field_.d2h_update();
#endif
        }

      private:
        typename FieldType::storage_info_type metaData_;
        FieldType field_;
    };

    class TypeErasedFieldView;

    template < typename T >
    struct is_TypeErasedFieldView : boost::mpl::false_ {};

    template <>
    struct is_TypeErasedFieldView< TypeErasedFieldView > : boost::mpl::true_ {};

} // namespace internal

/**
 * @brief Type erased view of a GridTools field
 *
 * The field is captured by @b reference, hence modifying the field will also modify the original
 * field. Construction cost is O(1).
 *
 * The TypeErasedFieldView allows an uniform treatment of all Field types (see @ref Storage).
 *
 * @b Example:
 * @code{.cpp}
 * IJKMetaStorageType meta(32, 32, 80);
 * IJKRealField field("field", meta, Real(5.0));
 *
 * // Erase type (capture field by reference)
 * TypeErasedFieldView view(field);
 *
 * // Modify field
 * view(20, 20, 40) = 100;
 *
 * // Original field will be modified as well
 * assert(view(20, 20, 40) == field(20, 20, 40));
 * @endcode
 *
 * @ingroup DycoreUnittestCoreLibrary
 * @see TypeErasedField
 */
class TypeErasedFieldView {
  public:
    TypeErasedFieldView(const TypeErasedFieldView &) = default;
    TypeErasedFieldView &operator=(const TypeErasedFieldView &) = default;

    /**
     * @brief Create a TypeErasedFieldView from GridTools field (stores a reference to the field)
     */
    template < class FieldType >
    TypeErasedFieldView(const FieldType &field) {
        static_assert(!internal::is_TypeErasedFieldView< FieldType >::value, "FieldType is not a GridTools field");

        // The const cast is ugly here but the signature of this constructor needs to be the same as
        // the copy constructor. Hence, we need to capture the field by const ref.. maybe this can
        // be improved.
        base_ = std::make_shared< internal::TypeErasedFieldViewBase< FieldType > >(const_cast< FieldType & >(field));
    }

    /**
     * @brief Used by TypeErasedField::toView()
     */
    TypeErasedFieldView(std::shared_ptr< internal::TypeErasedFieldInterface > &base, bool /*unused*/) : base_(base) {}

    /**
     * @brief Access the field at position (i, j, k) and return a const reference of the held value
     */
    const Real &operator()(int i, int j, int k) const noexcept { return base_->access(i, j, k); }

    /**
     * @brief Access the field at position (i, j, k) and return a reference of the held value
     */
    Real &operator()(int i, int j, int k) noexcept { return base_->access(i, j, k); }

    /**
     * @brief Pointer to the storage
     */
    Real *data() noexcept { return base_->data(); }

    /**
     * @brief Const pointer to the storage
     */
    const Real *data() const noexcept { return base_->data(); }

    /**
     * @brief First dimension (i-direction) excluding halo-boundaries
     */
    int iSize() const noexcept { return base_->iSize(); }

    /**
     * @brief First dimension (i-direction) including halo-boundaries
     *
     * For non-padded fields this is equal to iSizePadded()
     */
    int iSizeNoHalo() const noexcept { return base_->iSizeNoHalo(); }

    /**
     * @brief First dimension (i-direction) including potential padding
     */
    int iSizePadded() const noexcept { return base_->iSizePadded(); }

    /**
     * @brief Second dimension (j-direction) including halo-boundaries
     */
    int jSize() const noexcept { return base_->jSize(); }

    /**
     * @brief First dimension (i-direction) including halo-boundaries
     *
     * For non-padded fields this is equal to jSizePadded()
     */
    int jSizeNoHalo() const noexcept { return base_->jSizeNoHalo(); }

    /**
     * @brief First dimension (i-direction) including potential padding
     */
    int jSizePadded() const noexcept { return base_->jSizePadded(); }

    /**
     * @brief Third dimension (k-direction)
     */
    int kSize() const noexcept { return base_->kSize(); }

    /**
     * @brief Third dimension (k-direction) including potential padding
     */
    int kSizePadded() const noexcept { return base_->kSizePadded(); }

    /**
     * @brief Total size of field the field including potential padding
     */
    int size() const noexcept { return iSize() * jSize() * kSize(); }

    /**
     * @brief Stride in i-direction
     */
    int iStride() const noexcept { return base_->iStride(); }

    /**
     * @brief Stride in j-direction
     */
    int jStride() const noexcept { return base_->jStride(); }

    /**
     * @brief Stride in k-direction
     */
    int kStride() const noexcept { return base_->kStride(); }

    /**
     * @brief Name of the field
     */
    std::string name() const noexcept { return std::string(base_->name()); }

    /**
     * @brief Update device pointer (calls h2d_update)
     */
    void updateDevice() const noexcept { base_->updateDevice(); }

    /**
     * @brief Update host pointer (calls d2h_update)
     */
    void updateHost() const noexcept { base_->updateHost(); }

  private:
    std::shared_ptr< internal::TypeErasedFieldInterface > base_;
};

/**
 * @brief Type erased field of a GridTools field
 *
 * The field is captured by @b value, hence copied in from the original field. Construction
 * cost is O(n) where n is the total size of the field.
 *
 * The TypeErasedFieldView allows an uniform treatement of all Field types (see @ref Storage).
 *
 * @b Example:
 * @code{.cpp}
 * IJKMetaStorageType meta(32, 32, 80);
 * IJKRealField field("field", meta, Real(5.0));
 *
 * // Erase type (capture field by value)
 * TypeErasedField copy_field(field);
 *
 * // Modify field
 * copy_field(20, 20, 40) = 100;
 *
 * // Original field is NOT modifed
 * assert(copy_field(20, 20, 40) != field(20, 20, 40));
 * @endcode
 *
 * @ingroup DycoreUnittestCoreLibrary
 * @see TypeErasedFieldView
 */
class TypeErasedField {
  public:
    TypeErasedField(const TypeErasedField &) = default;
    TypeErasedField &operator=(const TypeErasedField &) = default;

    /**
     * @brief Create a TypeErasedField by copying GridTools field
     */
    template < class FieldType >
    TypeErasedField(const FieldType &field) {
        base_ = std::make_shared< internal::TypeErasedFieldBase< FieldType > >(field);
    }

    /**
     * @brief Access the field at position (i, j, k) and return a const refrence of the held value
     */
    const Real &operator()(int i, int j, int k) const noexcept { return base_->access(i, j, k); }

    /**
     * @brief Access the field at position (i, j, k) and return a refrence of the held value
     */
    Real &operator()(int i, int j, int k) noexcept { return base_->access(i, j, k); }

    /**
     * @brief Pointer to the storage
     */
    Real *data() noexcept { return base_->data(); }

    /**
     * @brief Const pointer to the storage
     */
    const Real *data() const noexcept { return base_->data(); }

    /**
     * @brief First dimension (i-direction) excluding halo-boundaries
     */
    int iSize() const noexcept { return base_->iSize(); }

    /**
     * @brief First dimension (i-direction) including halo-boundaries
     *
     * For non-padded fields this is equal to iSizePadded()
     */
    int iSizeNoHalo() const noexcept { return base_->iSizeNoHalo(); }

    /**
     * @brief First dimension (i-direction) including potential padding
     */
    int iSizePadded() const noexcept { return base_->iSizePadded(); }

    /**
     * @brief Second dimension (j-direction) including halo-boundaries
     */
    int jSize() const noexcept { return base_->jSize(); }

    /**
     * @brief First dimension (i-direction) including halo-boundaries
     *
     * For non-padded fields this is equal to jSizePadded()
     */
    int jSizeNoHalo() const noexcept { return base_->jSizeNoHalo(); }

    /**
     * @brief First dimension (i-direction) including potential padding
     */
    int jSizePadded() const noexcept { return base_->jSizePadded(); }

    /**
     * @brief Third dimension (k-direction)
     */
    int kSize() const noexcept { return base_->kSize(); }

    /**
     * @brief Third dimension (k-direction) including potential padding
     */
    int kSizePadded() const noexcept { return base_->kSizePadded(); }

    /**
     * @brief Total size of field the field including potential padding
     */
    int size() const noexcept { return iSize() * jSizePadded() * kSize(); }

    /**
     * @brief Stride in i-direction
     */
    int iStride() const noexcept { return base_->iStride(); }

    /**
     * @brief Stride in j-direction
     */
    int jStride() const noexcept { return base_->jStride(); }

    /**
     * @brief Stride in k-direction
     */
    int kStride() const noexcept { return base_->kStride(); }

    /**
     * @brief Name of the field
     */
    std::string name() const noexcept { return std::string(base_->name()); }

    /**
     * @brief Update device pointer (calls h2d_update)
     */
    void updateDevice() noexcept { base_->updateDevice(); }

    /**
     * @brief Update host pointer (calls d2h_update)
     */
    void updateHost() const noexcept { base_->updateHost(); }

    /**
     * @brief Convert field to view
     */
    TypeErasedFieldView toView() noexcept { return TypeErasedFieldView(base_, true); }

  private:
    std::shared_ptr< internal::TypeErasedFieldInterface > base_;
};

GT_VERIFICATION_NAMESPACE_END
