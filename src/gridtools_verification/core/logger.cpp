/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "logger.h"
#include "command_line.h"
#include <cstdlib>
#include <iostream>

namespace gt_verification {

    logger *logger::instance_ = nullptr;

    logger &logger::getInstance() {
        if (!instance_)
            instance_ = new logger;
        return (*instance_);
    }

    logger::logger() : enable_(false), flushed_(true), outStream_(std::clog) {
        // Check environment variable
        const char *envDycoreLog = std::getenv("VERIFICATION_LOG"); // FIXME
        enable_ = envDycoreLog && (std::atoi(envDycoreLog) > 0);
    }
} // namespace gt_verification
