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
#include "../VerificationException.h"
#include "../core/Color.h"
#include "../core/CommandLine.h"
#include "../core/Error.h"
#include "../core/Utility.h"
#include "../core/Logger.h"
#include "ErrorMetric.h"
#include "VerificationSpecification.h"
#include <cstdlib>
#include <string>

GT_VERIFICATION_NAMESPACE_BEGIN

VerificationSpecification::VerificationSpecification(CommandLine &cl) {
    parse(cl.has("error") ? cl.as< std::string >("error") : std::string());
}

void VerificationSpecification::printHelp(char *currentExecutable) noexcept {
    const int maxTerminalSize = 80;
    const int indentSize = 5;

    auto printKeyword = [&](std::string keyword, std::string valueStr, std::string desc) {
        // Print Keyword in green
        cprintf(Color::GREEN, "  %s", keyword.c_str());

        // Print value string of the keyword in yellow (if provided)
        if (!valueStr.empty()) {
            cprintf(Color::GREEN, "=");
            cprintf(Color::YELLOW, "%s", valueStr.c_str());
        }

        // Print description
        std::printf("\n%s\n", splitString(desc, maxTerminalSize, indentSize).c_str());
    };

    // Print header
    cprintf(Color::BOLDWHITE, "\nUsage: %s --error=KEYWORDS\n\n", currentExecutable);
    std::cout << splitString("Set the behaviour on how errors are being reported. KEYWORDS is a "
                             "comma seperated list of string value pairs. The following "
                             "keywords are known:")
              << "\n\n";

    // Register Keywords
    printKeyword("field", "<string>", "Only print failures of field with name <string>.");
    printKeyword("list", "", "Print a list of the errors with their (i,j,k) position and value.");
    printKeyword("stop-on-error",
        "",
        "Abort after the first reported set of failures, which "
        "usually refers to a Verification::verify() run, by calling "
        "std::exit(1).");
    printKeyword("visualize", "", "Visualize the layers in ASCII art.");
    printKeyword("max-errors",
        "<int>",
        "Only print the first <int> errors (implies the keyword "
        "'list').");
    printKeyword("k",
        "<X>-<Y>",
        "Only report failures from the layers in the range [X, Y] where"
        " <X> and <Y> are two integers seperated by '-' in the range "
        "[0, kmax]. If <Y> is omitted then only the layer <X> is being"
        " reported. Example: k=5-10 or k=20.");
    printKeyword("atol",
        "<float>",
        (boost::format("Set the absolute tolerance of the error metric (default: %.0e).") % ErrorMetric::atolDefault)
            .str());
    printKeyword("rtol",
        "<float>",
        (boost::format("Set the relative tolerance of the error metric (default: %.0e).") % ErrorMetric::rtolDefault)
            .str());

    // Print example
    std::cout << "\nExample: --error=visualize,k=1-5,k=6" << std::endl;

    std::exit(EXIT_SUCCESS);
}

void VerificationSpecification::parse(std::string errorStr) {
    // 1. Set default value
    list_ = false;
    maxErrorsToList_ = -1;
    visualize_ = false;
    kIntervalSpecified_ = false;
    stopOnError_ = false;
    kInterval_.clear();

    // 2. Parse string
    if (!errorStr.empty()) {
        std::vector< std::string > tokens(tokenizeString(errorStr, ","));
        std::string keywordStr, valueStr;

        try {
            for (const auto &token : tokens) {
                auto posOfAssignment = token.find('=', 0);

                // Extract keyword
                keywordStr = token.substr(0, posOfAssignment);

                // Extract value (if any)
                valueStr.clear();
                if (posOfAssignment != std::string::npos)
                    valueStr = token.substr(posOfAssignment + 1);

                // field
                if (keywordStr == "field") {
                    if (valueStr.empty())
                        throw VerificationException(
                            "parsing error in '--error': missing argument of keyword '%s'", keywordStr);
                    fieldname_ = valueStr;
                    VERIFICATION_LOG() << "VerificationReporter: Parsing keyword 'field' as " << fieldname_
                                       << logger::endl;
                }
                // list
                else if (keywordStr == "list") {
                    if (!valueStr.empty())
                        throw VerificationException(
                            "parsing error in '--error': keyword '%s' cannot have an argument", keywordStr);
                    list_ = true;
                    VERIFICATION_LOG() << "VerificationReporter: Parsing keyword 'list' as true" << logger::endl;
                }
                // stop-on-error
                else if (keywordStr == "stop-on-error") {
                    if (!valueStr.empty())
                        throw VerificationException(
                            "parsing error in '--error': keyword '%s' cannot have an argument", keywordStr);
                    stopOnError_ = true;
                    VERIFICATION_LOG() << "VerificationReporter: Parsing keyword 'stop-on-error' as true"
                                       << logger::endl;
                }
                // visualize
                else if (keywordStr == "visualize") {
                    if (!valueStr.empty())
                        throw VerificationException(
                            "parsing error in '--error': keyword '%s' cannot have an argument", keywordStr);
                    visualize_ = true;
                    VERIFICATION_LOG() << "VerificationReporter: Parsing keyword 'visualize' as true" << logger::endl;
                }
                // max-errors
                else if (keywordStr == "max-errors") {
                    if (valueStr.empty())
                        throw VerificationException(
                            "parsing error in '--error': missing argument of keyword '%s'", keywordStr);
                    maxErrorsToList_ = std::atoi(valueStr.c_str());
                    VERIFICATION_LOG() << "VerificationReporter: Parsing keyword 'max-errors' as " << maxErrorsToList_
                                       << logger::endl;
                }
                // atol
                else if (keywordStr == "atol") {
                    if (valueStr.empty())
                        throw VerificationException(
                            "parsing error in '--error': missing argument of keyword '%s'", keywordStr);
                    ErrorMetric::atolDefault = std::atof(valueStr.c_str());
                    VERIFICATION_LOG() << "VerificationReporter: Parsing keyword 'atol' as " << ErrorMetric::atolDefault
                                       << logger::endl;
                }
                // rtol
                else if (keywordStr == "rtol") {
                    if (valueStr.empty())
                        throw VerificationException(
                            "parsing error in '--error': missing argument of keyword '%s'", keywordStr);
                    ErrorMetric::rtolDefault = std::atof(valueStr.c_str());
                    VERIFICATION_LOG() << "VerificationReporter: Parsing keyword 'rtol' as " << ErrorMetric::rtolDefault
                                       << logger::endl;
                }
                // k
                else if (keywordStr == "k") {
                    if (valueStr.empty())
                        throw VerificationException(
                            "parsing error in '--error': missing argument of keyword '%s'", keywordStr);

                    auto posOfDelim = valueStr.find('-', 0);

                    // Add a range (e.g 5-10 will add {5, 6, 7, 8, 9, 10})
                    if (posOfDelim != std::string::npos) {
                        int kStart = std::atoi(valueStr.substr(0, posOfDelim).c_str());
                        int kEnd = std::atoi(valueStr.substr(posOfDelim + 1).c_str());
                        for (int k = kStart; k <= kEnd; ++k)
                            kInterval_.push_back(k);
                    }
                    // Add a single value
                    else
                        kInterval_.push_back(std::atoi(valueStr.c_str()));

                    auto &logStream = VERIFICATION_LOG();
                    logStream << "VerificationReporter: Parsing keyword 'k' as { ";
                    for (int k : kInterval_)
                        logStream << k << " ";
                    logStream << "} " << logger::endl;
                } else
                    throw VerificationException(
                        "parsing error in '--error': unrecognised keyword '%s'", keywordStr.empty() ? "," : keywordStr);
            }
        } catch (VerificationException &dycoreException) {
            Error::fatal(dycoreException.what());
        }
    }

    // 3. Adjust values (resolve dependencies)
    if (maxErrorsToList_ > 0)
        list_ = true;
    else if (maxErrorsToList_ < 0 && list_)
        maxErrorsToList_ = std::numeric_limits< int >::max();

    kIntervalSpecified_ = !kInterval_.empty();
}

GT_VERIFICATION_NAMESPACE_END
