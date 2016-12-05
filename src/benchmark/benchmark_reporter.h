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
#include "../core.h"
#include "../common.h"
#include "benchmark_specification.h"

GT_VERIFICATION_NAMESPACE_BEGIN

class Dycore;

/**
 * @brief Report results of the benchmarks to JSON
 *
 * @ingroup DycoreUnittestBenchmarkLibrary
 */
class benchmark_reporter : private boost::noncopyable {
  public:
    /**
     * @brief Initialize reporter
     *
     * @param name  Name of the benchmark
     */
    benchmark_reporter(std::string name, benchmark_specification benchmarkSpecification);

    /**
     * @brief Add a measurment
     *
     * @param time  Measured time in seconds
     */
    inline void add_measurement(double time) noexcept { measurements_.push_back(time); }

    /**
     * @brief Report results of the benchmark
     *
     * This will report the mean of the measurements as well as the standard deviation.
     * @see benchmark_specification
     */
    void report();

    /**
     * @brief Print configuration of the current benchmark run.
     *
     * @param dycore  The dycore object in use
     */
    void print_header() noexcept;

  private:
    std::string name_;
    benchmark_specification benchmarkSpecification_;
    std::vector< double > measurements_;
};

GT_VERIFICATION_NAMESPACE_END
