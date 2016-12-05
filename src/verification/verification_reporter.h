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

#include <vector>
#include <string>
#include "common.h"
#include "verification_specification.h"
#include "verification.h"

GT_VERIFICATION_NAMESPACE_BEGIN

/**
 * @brief Report errors to console
 *
 * @ingroup DycoreUnittestVerificationLibrary
 */
class verification_reporter : private boost::noncopyable {
  protected:
    void list_failures(const verification &Verification) const noexcept;
    void visualize_failures(const verification &Verification) const noexcept;

  public:
    verification_reporter(const verification_specification verifSpec) : verifSpec_(verifSpec){};

    /**
     * @brief Report failures to console
     *
     * The behaviour depends on the passed error string to the command-line option @c --error.
     *
     * @param verification  The verification object storing potential failures
     *
     * @see VerificationSpecification
     */
    void report(const verification &Verification) const noexcept;

  private:
    verification_specification verifSpec_;
};

GT_VERIFICATION_NAMESPACE_END
