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
#include <initializer_list>
#include "../VerificationException.h"
#include "../Core.h"
#include "BenchmarkSpecification.h"

GT_VERIFICATION_NAMESPACE_BEGIN

void BenchmarkSpecification::printHelp(char *currentExecutable) noexcept {
    const int maxTerminalSize = 80;
    const int indentSize = 5;

    auto printKeyword = [&](std::string keyword, std::string valueStr, std::initializer_list< std::string > desc) {
        // Print Keyword in green
        cprintf(Color::GREEN, "  %s", keyword.c_str());

        // Print value string of the keyword in yellow (if provided)
        if (!valueStr.empty()) {
            cprintf(Color::GREEN, "=");
            cprintf(Color::YELLOW, "%s", valueStr.c_str());
        }

        // Print description
        for (auto d : desc)
            std::printf("\n%s", splitString(d, maxTerminalSize, indentSize).c_str());
        std::printf("\n");
    };

    // Print header
    cprintf(Color::BOLDWHITE, "\nUsage: %s --benchmark=KEYWORDS\n\n", currentExecutable);
    std::cout << splitString("Specify how benchmarks are being executed. KEYWORDS is a "
                             "comma seperated list of string value pairs. The following "
                             "keywords are known:")
              << "\n\n";

    // Register Keywords
    printKeyword("domain",
        "<X>:<Y>:<Z>",
        {"Integer triple of the domain dimensions (without the "
         "halo sizes!) where <X> represents the i-dimension, <Y> "
         "the j-dimension and <Z> the k-dimension.",
            "Example: domain=25:25:80"});
    printKeyword("reps", "<int>", {"Number of repetitions of each stencil."});
    printKeyword("quiet", "", {"Dont ouput to stdout."});
    printKeyword("cache-size", "<int>", {"Size of last level cache in bytes [default: 8MB]."});
    printKeyword("file",
        "<name>:<mode>",
        {"The output will be written as JSON to the file <name>. The file replacing policy can "
         "optionally be set by providing a file-mode <mode>:",
            " - A : Insert into file if it exists or create a new one [default]",
            " - N : Create a new file and discard any content if it existed",
            "Example: file=myfile.json:A"});

    // Print example
    std::printf("\nExample: --benchmark=domain=30:30:80,reps=100\n");

    std::exit(EXIT_SUCCESS);
}

BenchmarkSpecification::BenchmarkSpecification(CommandLine &cl) {
    std::string benchmarkStr;

    if (cl.has("benchmark"))
        benchmarkStr = cl.as< std::string >("benchmark");

    // 1. Set default value
    iSize_ = 122;
    jSize_ = 122;
    kSize_ = 80;
    repetitions_ = 10;
    quiet_ = false;
    file_ = "";
    fileMode_ = EAppend;
    cacheSize_ = 8388608; // 8 MB

    // 2. Parse string
    if (!benchmarkStr.empty()) {
        std::vector< std::string > tokens(tokenizeString(benchmarkStr, ","));
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

                // reps
                if (keywordStr == "reps") {
                    if (valueStr.empty())
                        throw VerificationException(
                            "parsing error in '--benchmark': missing argument of keyword '%s'", keywordStr);
                    repetitions_ = std::atoi(valueStr.c_str());
                    VERIFICATION_LOG() << "Benchmark: Parsing keyword 'reps' as " << repetitions_ << logger::endl;
                }
                // cache-size
                else if (keywordStr == "cache-size") {
                    if (valueStr.empty())
                        throw VerificationException(
                            "parsing error in '--benchmark': missing argument of keyword '%s'", keywordStr);
                    cacheSize_ = std::atoi(valueStr.c_str());
                    VERIFICATION_LOG() << "Benchmark: Parsing keyword 'cache-size' as " << cacheSize_ << logger::endl;
                }
                // quiet
                else if (keywordStr == "quiet") {
                    if (!valueStr.empty())
                        throw VerificationException(
                            "parsing error in '--benchmark': keyword '%s' cannot have an argument", keywordStr);
                    quiet_ = true;
                    VERIFICATION_LOG() << "Benchmark: Parsing keyword 'quiet' as true" << logger::endl;
                }
                // file
                else if (keywordStr == "file") {
                    if (valueStr.empty())
                        throw VerificationException(
                            "parsing error in '--benchmark': missing argument of keyword '%s'", keywordStr);

                    std::vector< std::string > fileTokens(tokenizeString(valueStr, ":"));

                    file_ = fileTokens[0];

                    if (fileTokens.size() > 1) {
                        switch (fileTokens[1][0]) {
                        case 'A':
                            fileMode_ = EAppend;
                            break;
                        case 'N':
                            fileMode_ = ENew;
                            break;
                        default:
                            throw VerificationException("parsing error in '--benchmark': invalid file-mode "
                                                        "'%s' in argument '%s'",
                                fileTokens[1],
                                valueStr);
                        }
                    }

                    VERIFICATION_LOG() << "Benchmark: Parsing keyword 'file' as " << file_ << logger::endl;
                }
                // domain
                else if (keywordStr == "domain") {
                    if (valueStr.empty())
                        throw VerificationException(
                            "parsing error in '--benchmark': missing argument of keyword '%s'", keywordStr);

                    std::vector< std::string > domainTokens(tokenizeString(valueStr, ":"));
                    if (domainTokens.size() != 3)
                        throw VerificationException("parsing error in '--benchmark': invalid format '%s' of "
                                                    "keyword '%s', expected triple of the form <X>:<Y>:<Z>",
                            valueStr,
                            keywordStr);

                    iSize_ = std::atoi(domainTokens[0].c_str());
                    jSize_ = std::atoi(domainTokens[1].c_str());
                    kSize_ = std::atoi(domainTokens[2].c_str());

                    if (iSize_ < 1 || jSize_ < 1 || kSize_ < 1)
                        throw VerificationException("parsing error in '--benchmark': invalid domain '%s'", valueStr);

                    VERIFICATION_LOG() << "Benchmark: Parsing keyword 'domain' as {" << iSize_ << ", " << jSize_ << ", "
                                       << kSize_ << "}" << logger::endl;
                } else
                    throw VerificationException("parsing error in '--benchmark': unrecognised keyword '%s'",
                        keywordStr.empty() ? "," : keywordStr);
            }
        } catch (VerificationException &dycoreException) {
            Error::fatal(dycoreException.what());
        }
    }
}

std::string BenchmarkSpecification::toString() noexcept {
    std::stringstream sout;
    sout << boost::format("  %-28s %i\n") % "repetitions" % repetitions_;
    sout << boost::format("  %-28s %i\n") % "cacheSize" % cacheSize_;
    sout << boost::format("  %-28s %s\n") % "quiet" % (quiet_ ? "true" : "false");
    sout << boost::format("  %-28s %s\n") % "file" % file_;
    sout << boost::format("  %-28s %s\n") % "fileMode" % (fileMode_ == EAppend ? "EAppend" : "ENew");
    return sout.str();
}
GT_VERIFICATION_NAMESPACE_END
