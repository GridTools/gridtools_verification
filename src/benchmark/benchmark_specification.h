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

#include "../core.h"
#include "../common.h"

GT_VERIFICATION_NAMESPACE_BEGIN

/**
 * @brief Specify how benchmarks are being executed.
 *
 * The information is passed via command_line (`--benchmark`) as a single string of a comma seperated
 * list of `keyword-value` pairs.
 * Use `./DycoreBenchmark --benchmark=help` for a detailed description on the keyword-value pairs.
 *
 * @b Example:
 * @code
 * ./DycoreBenchmark --benchmark=domain=250:250:80,file=my_stencil_run.json
 * @endcode
 *
 * @ingroup DycoreUnittestBenchmarkLibrary
 */
class benchmark_specification {
  public:
    benchmark_specification(command_line &cl);

    /**
     * @brief Mode on how to open files
     */
    enum EFileMode {
        EAppend, /**< Append to file if it exists or create a new one [keyword: A] */
        ENew     /**< Create a new file and discard any content if it existed [keyword: N] */
    };

    /**
     * @brief Print the help information about the keywords and exit the program with
     *        EXIT_SUCCESS(0)
     */
    static void print_help(char *currentExecutable) noexcept;

    /**
     * @brief Size in i-direction (@b excluding halo boundaries)
     *
     * @code
     * ./DycoreBenchmark --benchmark=domain=250:250:80
     * @endcode
     */
    unsigned int i_size() const noexcept { return iSize_; }

    /**
     * @brief Size in j-direction (@b excluding halo boundaries)
     *
     * @code
     * ./DycoreBenchmark --benchmark=domain=250:250:80
     * @endcode
     */
    unsigned int j_size() const noexcept { return jSize_; }

    /**
     * @brief Size in k-direction
     *
     * @code
     * ./DycoreBenchmark --benchmark=domain=250:250:80
     * @endcode
     */
    unsigned int k_size() const noexcept { return kSize_; }

    /**
     * @brief Number of repetitions performed by each Stencil
     *
     * @code
     * ./DycoreBenchmark --benchmark=reps=10
     * @endcode
     */
    int repetitions() const noexcept { return repetitions_; }

    /**
     * @brief Size of last level cache in bytes
     *
     * @code
     * ./DycoreBenchmark --benchmark=cache-size=8388608
     * @endcode
     */
    int cache_size() const noexcept { return cacheSize_; }

    /**
     * @brief Dont ouput to stdout
     *
     * @code
     * ./DycoreBenchmark --benchmark=quiet
     * @endcode
     */
    bool quiet() const noexcept { return quiet_; }

    /**
     * @brief If not empty append result to file
     *
     * @code
     * ./DycoreBenchmark --benchmark=file=my_run.json:N
     * @endcode
     */
    std::string file() const noexcept { return file_; }

    /**
     * @brief Mode to open file @see EFileMode
     *
     * @code
     * ./DycoreBenchmark --benchmark=file=my_run.json:N
     * @endcode
     */
    EFileMode file_mode() const noexcept { return fileMode_; }

    /**
     * @brief Convert specification to multi-line string
     */
    std::string to_string() noexcept;

  private:
    unsigned int iSize_, jSize_, kSize_;
    int repetitions_;
    int cacheSize_;
    bool quiet_;
    std::string file_;
    EFileMode fileMode_;
};

GT_VERIFICATION_NAMESPACE_END
