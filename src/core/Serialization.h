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

#include <Serialbox/Savepoint.h>
#include <Serialbox/Serializer.h>
#include <Serialbox/SerializationException.h>
#include "../Common.h"
#include "CommandLine.h"
#include "TypeErasedField.h"

GT_VERIFICATION_NAMESPACE_BEGIN

/**
 * @brief Load and store GridTools fields from disk
 *
 * This is a wrapper around Serialbox.
 *
 * @ingroup DycoreUnittestCoreLibrary
 */
class Serialization : private boost::noncopyable {
  public:
    /**
     * @brief Initialize the Serialization object with a refrence serializer
     */
    Serialization(std::shared_ptr< ser::Serializer > serializer) : serializer_(serializer) {}

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
    void load(const std::string &name, FieldType &field, const ser::Savepoint &savepoint) {
        this->load(name, TypeErasedFieldView(field), savepoint);
    }

    void load(const std::string &name, TypeErasedFieldView field, const ser::Savepoint &savepoint);
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
     * @{
     */
    template < class FieldType >
    void write(std::string name, FieldType &field, const ser::Savepoint &savepoint) {
        this->write(name, TypeErasedFieldView(field), savepoint);
    }

    void write(std::string name, TypeErasedFieldView field, const ser::Savepoint &savepoint);
    /** @} */

    /**
     * Get the refrence serializer
     */
    std::shared_ptr< ser::Serializer > serializer() const noexcept { return serializer_; }

  private:
    std::shared_ptr< ser::Serializer > serializer_;
};

GT_VERIFICATION_NAMESPACE_END
