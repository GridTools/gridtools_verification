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

#include <string>
#include <vector>
#include "../Common.h"

GT_VERIFICATION_NAMESPACE_BEGIN

/**
 * @brief Split a string over multiple lines
 *
 * A new-line character will be inserted every @c lineSize characters. The new-line character
 * will be placed at the nearest whitespace position such that no word will be split. Every line
 * will be indented by @c indentSize.
 *
 * @param str           String to split
 * @param lineSize      Size of a line (usallly terminal size)
 * @param indentSize    Each line will be indented by indentSize
 *
 * @ingroup DycoreUnittestCoreLibrary
 */
std::string splitString(const std::string &str, std::size_t lineSize = 80, std::size_t indentSize = 0) noexcept;

/**
 * @brief Tokenize a string given the separation character @c delim
 *
 * The tokens will be stored in a vector of strings while the original delimiter characters will be
 * dropped.
 *
 * @param str       String to tokenize
 * @param delim     Delimiter character(s)
 *
 * @ingroup DycoreUnittestCoreLibrary
 */
std::vector< std::string > tokenizeString(const std::string &str, std::string delim) noexcept;

GT_VERIFICATION_NAMESPACE_END
