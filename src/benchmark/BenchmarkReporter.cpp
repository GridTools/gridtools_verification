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
#include "BenchmarkReporter.h"
#include "../VerificationException.h"
#include "../Core.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <streambuf>
#include <numeric>

#ifndef NDEBUG
#define NDBENG_WAS_NOT_DEFINED
#define NDEBUG
#endif
#include <STELLA/libjson/libjson.h>
#ifdef NDBENG_WAS_NOT_DEFINED
#undef NDEBUG
#endif

GT_VERIFICATION_NAMESPACE_BEGIN

BenchmarkReporter::BenchmarkReporter(std::string name, BenchmarkSpecification benchmarkSpecification)
    : name_(name), benchmarkSpecification_(benchmarkSpecification) {
    measurements_.reserve(benchmarkSpecification_.repetitions());
}

void BenchmarkReporter::report() {
    if (measurements_.empty())
        throw VerificationException("BenchmarkReporter of '%s': no measurements found", name_);

    // Extract bechmark and stencil name
    auto benchmarkDotStencil = tokenizeString(name_, ".");

    if (benchmarkDotStencil.size() != 2)
        throw VerificationException("invalid benchmark name, expected 'Benchmark.Stencil'");

    std::string benchmarkName = benchmarkDotStencil[0];
    std::string stencilName = benchmarkDotStencil[1];

    // Compute median
    std::sort(measurements_.begin(), measurements_.end());
    std::size_t n = measurements_.size();
    double medianRuntime = n % 2 == 0 ? 0.5 * (measurements_[n / 2 + 1] + measurements_[n / 2]) : measurements_[n / 2];
    (void)medianRuntime;

    // Compute mean
    double meanRuntime = std::accumulate(measurements_.begin(), measurements_.end(), 0.0) / n;

    // Compute standard deviation
    std::vector< double > diff(measurements_.size());
    std::transform(measurements_.begin(),
        measurements_.end(),
        diff.begin(),
        [meanRuntime](double x) -> double { return (x - meanRuntime); });
    double squaredSum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    double stdevRuntime = std::sqrt(squaredSum / n);

    // Stringify domain
    std::string domain(std::to_string(benchmarkSpecification_.iSize()) + " " +
                       std::to_string(benchmarkSpecification_.jSize()) + " " +
                       std::to_string(benchmarkSpecification_.kSize()));

    // Report to stdout
    if (!benchmarkSpecification_.quiet())
        std::printf("  %-23s     %18.10f   +/- %15.10f [s]\n", stencilName.c_str(), meanRuntime, stdevRuntime);

    // Report to file?
    std::string fileName(benchmarkSpecification_.file());
    if (fileName.empty())
        return;

    // Append to old benchmark?
    JSONNode root(JSON_NODE);
    try {
        std::string fileContent;

        std::ifstream fin(fileName);
        if (fin.good()) {
            VERIFICATION_LOG() << "Opening file: '" << fileName << "'" << logger::endl;
            fileContent.insert(
                fileContent.begin(), std::istreambuf_iterator< char >(fin), std::istreambuf_iterator< char >());

            // Load JSON from file
            if (!fileContent.empty() && (benchmarkSpecification_.fileMode() != BenchmarkSpecification::ENew)) {
                VERIFICATION_LOG() << "Parsing JSON of file: '" << fileName << "'" << logger::endl;
                root = libjson::parse(fileContent);
            }
        }
        fin.close();
    } catch (std::invalid_argument &ia) {
        Error::fatal("libJSON parsing error");
    }

    // Update the JSON file
    bool insertedDomain = false;
    JSONNode::iterator benchmarkIterator = root.begin();
    while (benchmarkIterator != root.end()) {
        // Benchmark is present?
        if (benchmarkIterator->name() == benchmarkName) {
            VERIFICATION_LOG() << "Found already existing benchmark '" << benchmarkName << "'" << logger::endl;

            // Stencil is present?
            JSONNode::iterator stencilIterator = benchmarkIterator->begin();
            while (stencilIterator != benchmarkIterator->end()) {
                if (stencilIterator->name() == stencilName) {
                    VERIFICATION_LOG() << "Found already existing stencil '" << stencilName << "'" << logger::endl;

                    JSONNode::iterator domainIterator = stencilIterator->begin();

                    // Append time to existing domain
                    while (domainIterator != stencilIterator->end()) {
                        if (domainIterator->name() == domain) {
                            VERIFICATION_LOG() << "Extending domain '" << domain << "' of stencil '" << stencilName
                                               << "'" << logger::endl;

                            insertedDomain = true;
                            domainIterator->push_back(JSONNode("", meanRuntime));
                        }
                        ++domainIterator;
                    }

                    // Insert new domain
                    if (!insertedDomain) {
                        VERIFICATION_LOG() << "Inserting domain '" << domain << "' into stencil '" << stencilName << "'"
                                           << logger::endl;

                        insertedDomain = true;
                        JSONNode domainNode(JSON_ARRAY);
                        domainNode.set_name(domain);
                        domainNode.push_back(JSONNode("", meanRuntime));
                        stencilIterator->push_back(domainNode);
                    }
                    break;
                }
                ++stencilIterator;
            }

            if (insertedDomain)
                break;

            // Insert new stencil
            insertedDomain = true;
            JSONNode stencil(JSON_NODE);
            stencil.set_name(stencilName);

            VERIFICATION_LOG() << "Inserting node '" << domain << "' into stencil '" << stencilName << "'"
                               << logger::endl;

            JSONNode domainNode(JSON_ARRAY);
            domainNode.set_name(domain);
            domainNode.push_back(JSONNode("", meanRuntime));
            stencil.push_back(domainNode);

            VERIFICATION_LOG() << "Inserting stencil '" << stencilName << "' into benchmark '" << benchmarkName << "'"
                               << logger::endl;

            benchmarkIterator->push_back(stencil);
        }
        ++benchmarkIterator;
    }

    // Create new benchmark
    if (!insertedDomain) {
        JSONNode benchmark(JSON_NODE);
        benchmark.set_name(benchmarkName);

        JSONNode stencil(JSON_NODE);
        stencil.set_name(stencilName);

        // Insert domain into stencil
        VERIFICATION_LOG() << "Inserting node '" << domain << "' into stencil '" << stencilName << "'" << logger::endl;

        JSONNode domainNode(JSON_ARRAY);
        domainNode.set_name(domain);
        domainNode.push_back(JSONNode("", meanRuntime));
        stencil.push_back(domainNode);

        // Insert stencil into benchmark
        VERIFICATION_LOG() << "Inserting stencil '" << stencilName << "' into benchmark '" << benchmarkName << "'"
                           << logger::endl;

        benchmark.push_back(stencil);

        // Insert benchmark into root
        root.push_back(benchmark);
    }

    // Write to file
    std::ofstream fout;
    fout.open(fileName, std::ofstream::out | std::ofstream::trunc);
    VERIFICATION_LOG() << "Writing to file: '" << fileName << "'" << logger::endl;
    fout << root.write_formatted() << std::endl;
    fout.close();
}

GT_VERIFICATION_NAMESPACE_END
