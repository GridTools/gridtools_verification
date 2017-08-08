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
#include <boost/type_traits.hpp>
#include "../common.h"

// FIXME forward
namespace gridtools {
    template < typename T, bool B >
    struct hybrid_pointer;
}

GT_VERIFICATION_NAMESPACE_BEGIN

namespace internal {

    /**
     * Type erased interface for GridTools fields with value_type T
     */
    template < typename T >
    class type_erased_field_interface : private boost::noncopyable {
      public:
        virtual ~type_erased_field_interface() {}

        /**
         * Access the field at position (i, j, k) and return a const refrence of the held value
         */
        virtual const T &access(int i, int j, int k) const noexcept = 0;

        /**
         * Access the field at position (i, j, k) and return a refrence of the held value
         */
        virtual T &access(int i, int j, int k) noexcept = 0;

        /**
         * Pointer to the storage
         */
        virtual T *data() noexcept = 0;

        /**
         * Const pointer to the storage
         */
        virtual const T *data() const noexcept = 0;

        /**
         * Get the name of the field
         */
        virtual const char *name() const noexcept = 0;

        /**
         * First dimension (i-direction) excluding halo-boundaries
         */
        virtual int i_size() const noexcept = 0;

        /**
         * First dimension (i-direction) including potential padding
         */
        virtual int i_size_padded() const noexcept = 0;

        /**
         * Second dimension (j-direction) including halo-boundaries
         */
        virtual int j_size() const noexcept = 0;

        /**
         * First dimension (i-direction) including potential padding
         */
        virtual int j_size_padded() const noexcept = 0;

        /**
         * Third dimension (k-direction)
         */
        virtual int k_size() const noexcept = 0;

        /**
         * Third dimension (k-direction) including potential padding
         */
        virtual int k_size_padded() const noexcept = 0;

        /**
         * Stride in i-direction
         */
        virtual int i_stride() const noexcept = 0;

        /**
         * Stride in j-direction
         */
        virtual int j_stride() const noexcept = 0;

        /**
         * Stride in k-direction
         */
        virtual int k_stride() const noexcept = 0;

        /**
         * Sync host and device
         */
        virtual void sync() noexcept = 0;
    };

    template < typename FieldType, typename T >
    class type_erased_field_view_base : public type_erased_field_interface< T > {
      public:
        static_assert(
            std::is_same< typename FieldType::storage_t::data_t, T >::value, "internal error: types do not match");

        type_erased_field_view_base(FieldType &field) : field_(field) {}

        virtual const T &access(int i, int j, int k) const noexcept override { return make_host_view(field_)(i, j, k); }

        virtual T &access(int i, int j, int k) noexcept override { return make_host_view(field_)(i, j, k); }

        virtual T *data() noexcept override { return &make_host_view(field_)(0, 0, 0); }

        virtual const T *data() const noexcept override { return &make_host_view(field_)(0, 0, 0); }

        virtual const char *name() const noexcept override { return field_.name().c_str(); }

        virtual int i_size() const noexcept override {
            return field_.get_storage_info_ptr()->template unaligned_dim< 0 >();
        }

        virtual int i_size_padded() const noexcept override {
            return field_.get_storage_info_ptr()->template dim< 0 >();
        }

        virtual int j_size() const noexcept override {
            return field_.get_storage_info_ptr()->template unaligned_dim< 1 >();
        }

        virtual int j_size_padded() const noexcept override {
            return field_.get_storage_info_ptr()->template dim< 1 >();
        }

        virtual int k_size() const noexcept override {
            return field_.get_storage_info_ptr()->template unaligned_dim< 2 >();
        }

        virtual int k_size_padded() const noexcept override {
            return field_.get_storage_info_ptr()->template dim< 2 >();
        }

        virtual int i_stride() const noexcept override { return field_.get_storage_info_ptr()->template stride< 0 >(); }

        virtual int j_stride() const noexcept override { return field_.get_storage_info_ptr()->template stride< 1 >(); }

        virtual int k_stride() const noexcept override { return field_.get_storage_info_ptr()->template stride< 2 >(); }

        virtual void sync() noexcept override { field_.sync(); }

      private:
        FieldType &field_;
    };

    template < typename FieldType, typename T >
    class type_erased_field_base : public type_erased_field_interface< T > {
      public:
        static_assert(
            std::is_same< typename FieldType::storage_t::data_t, T >::value, "internal error: types do not match");

        type_erased_field_base(const FieldType &field)
            : metaData_(field.get_storage_info_ptr()->template unaligned_dim< 0 >(),
                  field.get_storage_info_ptr()->template unaligned_dim< 1 >(),
                  field.get_storage_info_ptr()->template unaligned_dim< 2 >()),
              field_(metaData_, -1, field.name()) {

            // Update field on host
            field_.sync();

            // Copy field
            int iSize = this->i_size();
            int jSize = this->j_size();
            int kSize = this->k_size();
            auto src = make_host_view(field);
            auto dst = make_host_view(field_);
            for (int i = 0; i < iSize; ++i)
                for (int j = 0; j < jSize; ++j)
                    for (int k = 0; k < kSize; ++k)
                        dst(i, j, k) = src(i, j, k);
        }

        virtual const T &access(int i, int j, int k) const noexcept override { return make_host_view(field_)(i, j, k); }

        virtual T &access(int i, int j, int k) noexcept override { return make_host_view(field_)(i, j, k); }

        virtual T *data() noexcept override { return &make_host_view(field_)(0, 0, 0); }

        virtual const T *data() const noexcept override { return &make_host_view(field_)(0, 0, 0); }

        virtual const char *name() const noexcept override { return field_.name().c_str(); }

        virtual int i_size() const noexcept override {
            return field_.get_storage_info_ptr()->template unaligned_dim< 0 >();
        }

        virtual int i_size_padded() const noexcept override {
            return field_.get_storage_info_ptr()->template dim< 0 >();
        }

        virtual int j_size() const noexcept override {
            return field_.get_storage_info_ptr()->template unaligned_dim< 1 >();
        }

        virtual int j_size_padded() const noexcept override {
            return field_.get_storage_info_ptr()->template dim< 1 >();
        }

        virtual int k_size() const noexcept override {
            return field_.get_storage_info_ptr()->template unaligned_dim< 2 >();
        }

        virtual int k_size_padded() const noexcept override {
            return field_.get_storage_info_ptr()->template dim< 2 >();
        }

        virtual int i_stride() const noexcept override { return field_.get_storage_info_ptr()->template stride< 0 >(); }

        virtual int j_stride() const noexcept override { return field_.get_storage_info_ptr()->template stride< 1 >(); }

        virtual int k_stride() const noexcept override { return field_.get_storage_info_ptr()->template stride< 2 >(); }

        virtual void sync() noexcept override { field_.sync(); }

      private:
        typename FieldType::storage_info_t metaData_;
        FieldType field_;
    };

    class type_erased_field_view;

    template < typename T >
    struct is_type_erased_field_view : boost::mpl::false_ {};

    template <>
    struct is_type_erased_field_view< type_erased_field_view > : boost::mpl::true_ {};

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
 * IJKRealField field("field", meta, T(5.0));
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
template < typename T >
class type_erased_field_view {
  public:
    type_erased_field_view(const type_erased_field_view &) = default;
    type_erased_field_view &operator=(const type_erased_field_view &) = default;

    /**
     * @brief Create a TypeErasedFieldView from GridTools field (stores a reference to the field)
     */
    template < class FieldType >
    type_erased_field_view(const FieldType &field) {
        static_assert(!internal::is_type_erased_field_view< FieldType >::value, "FieldType is not a GridTools field");

        // The const cast is ugly here but the signature of this constructor needs to be the same as
        // the copy constructor. Hence, we need to capture the field by const ref.. maybe this can
        // be improved.
        base_ =
            std::make_shared< internal::type_erased_field_view_base< FieldType, T > >(const_cast< FieldType & >(field));
    }

    /**
     * @brief Used by TypeErasedField::toView()
     */
    type_erased_field_view(std::shared_ptr< internal::type_erased_field_interface< T > > &base, bool /*unused*/)
        : base_(base) {}

    /**
     * @brief Access the field at position (i, j, k) and return a const reference of the held value
     */
    const T &operator()(int i, int j, int k) const noexcept { return base_->access(i, j, k); }

    /**
     * @brief Access the field at position (i, j, k) and return a reference of the held value
     */
    T &operator()(int i, int j, int k) noexcept { return base_->access(i, j, k); }

    /**
     * @brief Pointer to the storage
     */
    T *data() noexcept { return base_->data(); }

    /**
     * @brief Const pointer to the storage
     */
    const T *data() const noexcept { return base_->data(); }

    /**
     * @brief First dimension (i-direction) excluding halo-boundaries
     */
    int i_size() const noexcept { return base_->i_size(); }

    /**
     * @brief First dimension (i-direction) including potential padding
     */
    int i_size_padded() const noexcept { return base_->i_size_padded(); }

    /**
     * @brief Second dimension (j-direction) including halo-boundaries
     */
    int j_size() const noexcept { return base_->j_size(); }

    /**
     * @brief First dimension (i-direction) including potential padding
     */
    int j_size_padded() const noexcept { return base_->j_size_padded(); }

    /**
     * @brief Third dimension (k-direction)
     */
    int k_size() const noexcept { return base_->k_size(); }

    /**
     * @brief Third dimension (k-direction) including potential padding
     */
    int k_size_padded() const noexcept { return base_->k_size_padded(); }

    /**
     * @brief Total size of field the field including potential padding
     */
    int size() const noexcept { return i_size() * j_size() * k_size(); }

    /**
     * @brief Stride in i-direction
     */
    int i_stride() const noexcept { return base_->i_stride(); }

    /**
     * @brief Stride in j-direction
     */
    int j_stride() const noexcept { return base_->j_stride(); }

    /**
     * @brief Stride in k-direction
     */
    int k_stride() const noexcept { return base_->k_stride(); }

    /**
     * @brief Name of the field
     */
    std::string name() const noexcept { return std::string(base_->name()); }

    /**
     * @brief Sync host and device
     */
    void sync() const noexcept { base_->sync(); }

  private:
    std::shared_ptr< internal::type_erased_field_interface< T > > base_;
};

/**
 * @brief Type erased field of a GridTools field
 *
 * The field is captured by @b value, hence copied in from the original field. Construction
 * cost is O(n) where n is the total size of the field.
 *
 * The TypeErasedFieldView allows an uniform treatment of all Field types (see @ref Storage).
 *
 * @b Example:
 * @code{.cpp}
 * IJKMetaStorageType meta(32, 32, 80);
 * IJKRealField field("field", meta, T(5.0));
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
template < typename T >
class type_erased_field {
  public:
    type_erased_field(const type_erased_field &) = default;
    type_erased_field &operator=(const type_erased_field &) = default;

    /**
     * @brief Create a TypeErasedField by copying GridTools field
     */
    template < class FieldType >
    type_erased_field(const FieldType &field) {
        base_ = std::make_shared< internal::type_erased_field_base< FieldType, T > >(field);
    }

    /**
     * @brief Access the field at position (i, j, k) and return a const refrence of the held value
     */
    const T &operator()(int i, int j, int k) const noexcept { return base_->access(i, j, k); }

    /**
     * @brief Access the field at position (i, j, k) and return a refrence of the held value
     */
    T &operator()(int i, int j, int k) noexcept { return base_->access(i, j, k); }

    /**
     * @brief Pointer to the storage
     */
    T *data() noexcept { return base_->data(); }

    /**
     * @brief Const pointer to the storage
     */
    const T *data() const noexcept { return base_->data(); }

    /**
     * @brief First dimension (i-direction) excluding halo-boundaries
     */
    int i_size() const noexcept { return base_->i_size(); }

    /**
     * @brief First dimension (i-direction) including potential padding
     */
    int i_size_padded() const noexcept { return base_->i_size_padded(); }

    /**
     * @brief Second dimension (j-direction) including halo-boundaries
     */
    int j_size() const noexcept { return base_->j_size(); }

    /**
     * @brief First dimension (i-direction) including potential padding
     */
    int j_size_padded() const noexcept { return base_->j_size_padded(); }

    /**
     * @brief Third dimension (k-direction)
     */
    int k_size() const noexcept { return base_->k_size(); }

    /**
     * @brief Third dimension (k-direction) including potential padding
     */
    int k_size_padded() const noexcept { return base_->k_size_padded(); }

    /**
     * @brief Total size of field the field including potential padding
     */
    int size() const noexcept { return i_size() * j_size_padded() * k_size(); }

    /**
     * @brief Stride in i-direction
     */
    int i_stride() const noexcept { return base_->i_stride(); }

    /**
     * @brief Stride in j-direction
     */
    int j_stride() const noexcept { return base_->j_stride(); }

    /**
     * @brief Stride in k-direction
     */
    int k_stride() const noexcept { return base_->k_stride(); }

    /**
     * @brief Name of the field
     */
    std::string name() const noexcept { return std::string(base_->name()); }

    /**
     * @brief Sync host and device
     */
    void sync() const noexcept { base_->sync(); }

    /**
     * @brief Convert field to view
     */
    type_erased_field_view< T > to_view() noexcept { return type_erased_field_view< T >(base_, true); }

  private:
    std::shared_ptr< internal::type_erased_field_interface< T > > base_;
};

GT_VERIFICATION_NAMESPACE_END
