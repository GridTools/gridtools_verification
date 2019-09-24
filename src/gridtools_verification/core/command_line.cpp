/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "error.h"
#include "command_line.h"
#include "include_boost_format.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "logger.h"

namespace po = boost::program_options;

namespace gt_verification {

    command_line::command_line(int argc, const char *const argv[]) : desc_("Options") {
        desc_.add_options()
            // --help
            ("help", "Display this information.")
            // --version
            ("version", "Display version information.")
            // --path, -p
            ("path,p",
                po::value< std::string >(),
                "Specify the input path (absolute path) used by SerialBox. If nothing is "
                "specified the current path './' is used. This argument takes precedence "
                "over the environment variable.")
            // --log, -l
            ("log,l", "Enable verbose logging to std::clog.")
            // --error
            ("error",
                po::value< std::string >()->value_name("KEYWORDS"),
                "Set the behaviour on how errors are being reported. Type '--error=help' to get detailed "
                "information about the available keywords.")
            // --benchmark
            ("benchmark",
                po::value< std::string >()->value_name("KEYWORDS"),
                "Specify how benchmarks are being executed. Type '--benchmark=help' to get detailed "
                "information about the available keywords.");

        try {
            po::store(po::parse_command_line(argc, argv, desc_), variableMap_);
            po::notify(variableMap_);

            const char *envDycoreLocation = std::getenv("DYCORE_DATA_LOCATION");

            // The command-line option takes precedence over the environment variable
            if (!has("path") && envDycoreLocation) {
                // Insert the value of the environment variable into the variable map
                po::variable_value variableValue(boost::any(std::string(envDycoreLocation)), false);
                variableMap_.insert(std::make_pair("path", variableValue));
            }
        } catch (const std::exception &e) {
            error::fatal(boost::format("%s, for help type '%s --help'") % e.what() % argv[0]);
        }

        if (has("log"))
            logger::getInstance().enable();
    }

    void command_line::print_help(char *currentExecutable) const noexcept {
        std::cout << "Usage: " << currentExecutable << " [options] \n\n" << desc_ << "\n"
                  << "Environment variables:\n"
                  << boost::format("  %-22s %s.\n") % "DYCORE_DATA_LOCATION" %
                         "Alternative way of specifying the input path"
                  << boost::format("  %-22s %s.\n") % "VERIFICATION_LOG" % "Enable logging if value is positve"
                  << std::endl;
        std::exit(EXIT_SUCCESS);
    }
}
