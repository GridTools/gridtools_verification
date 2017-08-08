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

#include <storage/storage-facility.hpp>
#include <serialbox/core/frontend/gridtools/Serializer.h>
#include "../common.h"
#include "command_line.h"
#include "type_erased_field.h"
#include "../verification_exception.h"
#include "error.h"
#include "logger.h"

GT_VERIFICATION_NAMESPACE_BEGIN

namespace ser = serialbox::gridtools;

/**
 * @brief Load and store GridTools fields from disk
 *
 * This is a wrapper around Serialbox.
 *
 * @ingroup DycoreUnittestCoreLibrary
 */
class serialization : private boost::noncopyable {
  public:
    /**
     * @brief Initialize the Serialization object with a reference serializer
     */
    serialization(std::shared_ptr< ser::serializer > serializer) : serializer_(serializer) {}

    /**
     * @brief Load the a field and store it in the provided field
     *
     * The field will be synchronized to the host before being accessed. However, the field is
     * @b not synchronized back to the device in the end.
     *
     * @param name      Name of the desired field
     * @param field     Field in which the data is going to be loaded to
     * @param savepoint Savepoint to load from
     *
     * @throw Exception The provided field does not match the requested one.
     * @{
     */
    template < class FieldType >
    void load(const std::string &name, FieldType &field, const ser::savepoint &savepoint, bool alsoPrevious = false) {
        this->load(
            name, type_erased_field_view< typename FieldType::storage_t::data_t >(field), savepoint, alsoPrevious);
    }

    template < typename T >
    void load(const std::string &name,
        type_erased_field_view< T > field,
        const ser::savepoint &savepoint,
        bool alsoPrevious = false) {

        // Make sure data is on the Host
        field.sync();

        // Get info of serialized field
        const ser::field_meta_info &info = serializer_->get_field_meta_info(name);

        int iSizeHalo = field.i_size();
        int jSizeHalo = field.j_size();
        int kSize = field.k_size();

        VERIFICATION_LOG() << boost::format(" - loading %-15s (%2i, %2i, %2i)") % name % iSizeHalo % jSizeHalo % kSize
                           << logger_action::endl;

        // Check dimensions
        if ((info.dims()[0] != iSizeHalo) || (info.dims()[1] != jSizeHalo) || (info.dims()[2] != kSize))
            throw verification_exception("the requested field '%s' has a different size than the provided field.\n"
                                         "Registered as: (%i, %i, %i)\n"
                                         "Given     as: (%i, %i, %i)",
                name,
                info.dims()[0],
                info.dims()[1],
                info.dims()[2],
                iSizeHalo,
                jSizeHalo,
                kSize);

        // Check types
        if (info.type() != serialbox::ToTypeID< T >::value)
            throw verification_exception(
                "the requested field '%s' has a different type than the provided field.", name);

        // Deserialize field
        int iStride = field.i_stride();
        int jStride = field.j_stride();
        int kStride = field.k_stride();

        std::vector< int > strides{iStride, jStride, kStride};
        // TODO check alsoPrevious
        serializer_->read(name, savepoint, field.data(), strides);

        field.sync();
    }

    // TODO should look like this
    //    template < typename Field >
    //    void load_gt(const std::string &name, Field &field, const ser::savepoint &savepoint, bool alsoPrevious =
    //    false) {
    //        // Make sure data is on the Host
    //        field.sync();
    //
    //        serializer_->read(name, savepoint, field);
    //
    //        field.sync();
    //    }

    /** @} */

    /**
     * @brief Serializes a data field at given savepoint
     *
     * This will automatically register the field with the provided name and serialize the field
     * to disk. The field will be synchronized to the host (@c d2h_update) before being accessed.
     *
     * @param name      Name to of the field to register
     * @param field     Field which is going to be written to disk
     * @param savepoint Savepoint to write to
     *
     * @throw Exception The registration failed because of an inconsistent registration due
     *                  fields with same name and different properties or a field with the same name
     *                  is already saved at the savepoint
     */
    template < class FieldType >
    void write(std::string name, FieldType &field, const ser::savepoint &savepoint) {
        this->write(name, type_erased_field_view< typename FieldType::storage_t::data_t >(field), savepoint);
    }

    template < typename T >
    void write(std::string name, type_erased_field_view< T > field, const ser::savepoint &savepoint) {
        // Make sure data is on the Host
        field.sync();

        if (name.empty())
            name = field.name();

        int iSize = field.i_size();
        int jSize = field.j_size();
        int kSize = field.k_size();

        VERIFICATION_LOG() << boost::format("Serializing '%s'") % name << logger_action::endl;

        int iStride = field.i_stride();
        int jStride = field.j_stride();
        int kStride = field.k_stride();

        const int iMinusHaloSize = 3;
        const int iPlusHaloSize = 3;
        const int jMinusHaloSize = 3;
        const int jPlusHaloSize = 3;

        auto typeID = serialbox::ToTypeID< T >::value;

        try {
            // Register field
            serializer_->register_field(name, typeID, std::vector< int >{iSize, jSize, kSize});

            // Write field to disk
            std::vector< int > strides{iStride, jStride, kStride};
            serializer_->write(name, savepoint, field.data(), strides);
        } catch (ser::exception &serException) {
            std::string errmsg(serException.what());
            throw verification_exception(errmsg.substr(errmsg.find_first_of("Error:") + sizeof("Error:")).c_str());
        }
    }

    //    template < typename Field >
    //    void write_gt(std::string name, Field &field, const ser::savepoint &savepoint) {
    //        // Make sure data is on the Host
    //        field.sync();
    //
    //        if (name.empty())
    //            name = field.name();
    //
    //        VERIFICATION_LOG() << boost::format("Serializing '%s'") % name << logger_action::endl;
    //
    //        try {
    //            serializer_->write(name, savepoint, field);
    //        } catch (ser::exception &serException) {
    //            std::string errmsg(serException.what());
    //            throw verification_exception(errmsg.substr(errmsg.find_first_of("Error:") +
    //            sizeof("Error:")).c_str());
    //        }
    //    }

    /**
     * Get the reference serializer
     */
    std::shared_ptr< ser::serializer > serializer() const noexcept { return serializer_; }

  private:
    std::shared_ptr< ser::serializer > serializer_;
};

GT_VERIFICATION_NAMESPACE_END
