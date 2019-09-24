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

#include <string>
#include <vector>
#include "../common.h"

namespace gt_verification {

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
    std::string split_string(const std::string &str, std::size_t lineSize = 80, std::size_t indentSize = 0) noexcept;

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
    std::vector< std::string > tokenize_string(const std::string &str, std::string delim) noexcept;
}
