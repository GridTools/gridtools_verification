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
#include "error.h"
#include "command_line.h"
#include <boost/format.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "logger.h"

namespace po = boost::program_options;

GT_VERIFICATION_NAMESPACE_BEGIN

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

GT_VERIFICATION_NAMESPACE_END
