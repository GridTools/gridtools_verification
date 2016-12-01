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

#include <boost/format.hpp>
#include <chrono>
#include <cstdio>
#include <ostream>
#include "Common.h"

GT_VERIFICATION_NAMESPACE_BEGIN

/**
 * @defgroup Logger
 * @brief Log to terminal
 *
 * The logger can log every object which provides a stream operator<< for std::clog. The Logger
 * object will be created upon first invocation.
 *
 * Every invocation of the macro VERIFICATION_LOG() will print the current date-time and insert a
 * newline
 * character/flush the stream (if it has not been manually done by dycore::log::flush or
 * dycore::log::endl).
 *
 * The logger can be enabled by passing the CommandLine option --log (-l) or by setting the
 * environment variable VERIFICATION_LOG to a @b positive value.
 *
 * To use the logger:
 * @code{.cpp}
 *  VERIFICATION_LOG() << "Hello" << std::string(", world!") << dycore::logger::endl;
 *  // Results in:
 *  // [21:03:08.753] Hello, world!
 * @endcode
 *
 * @ingroup DycoreUnittestCoreLibrary
 */

/**
 * @enum logger
 * @ingroup Logger
 */
enum class logger {
    nop,   /**< Do nothing */
    flush, /**< Flush log stream */
    endl,  /**< Insert a newline character and flush log stream */
    newlog /**< Indicate if this is the first formatting operation on this log */
};

/**
 * @brief Trivial Logger
 * @ingroup Logger
 */
class Logger : private boost::noncopyable /* singleton */
{
    Logger();

  public:
    /**
     * @brief Return the instance of the Logger
     */
    static Logger &getInstance();

    /**
     * @brief Inserts data into the logging stream
     */
    template < class ValueType >
    Logger &operator<<(const ValueType &value) noexcept {
        if (enable_)
            this->logImpl(value);
        return (*this);
    }

    /**
     * @brief Inserts a special @c logger character to control the behaviour of the stream
     */
    Logger &operator<<(const logger &loggerAction) noexcept {
        if (enable_) {
            logger loggerSwitch = (loggerAction == logger::newlog && !flushed_) ? logger::endl : loggerAction;

            switch (loggerSwitch) {
            case logger::endl:
                outStream_.put(outStream_.widen('\n'));
            case logger::flush:
                outStream_.flush();
                flushed_ = true;
            case logger::nop:
            default:
                break;
            }
        }
        return (*this);
    }

    void enable() { enable_ = true; }
    void disable() { enable_ = false; }

  private:
    template < class ValueType >
    void logImpl(const ValueType &value) noexcept {
        if (flushed_) {
            // Get current date-time (up to ms accuracy)
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            auto now_ms = now.time_since_epoch();
            auto now_sec = std::chrono::duration_cast< std::chrono::seconds >(now_ms);
            auto tm_ms = std::chrono::duration_cast< std::chrono::milliseconds >(now_ms - now_sec);

            std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
            struct tm *localTime = std::localtime(&currentTime);

            outStream_ << boost::format("[%02i:%02i:%02i.%03i] ") % localTime->tm_hour % localTime->tm_min %
                              localTime->tm_sec % tm_ms.count();
        }
        outStream_ << value;
        flushed_ = false;
    }

  private:
    bool enable_;
    int flushed_;
    std::ostream &outStream_;

    static Logger *instance_;
};

/**
 * @def VERIFICATION_LOG
 * @brief Invoke the Logger
 *
 * @code{.cpp}
 *  VERIFICATION_LOG() << "Hello " << "world!" << logger::endl;
 *  // Results in:
 *  // [21:03:08.753] Hello, world!
 * @endcode
 *
 * @ingroup Logger
 */
#define VERIFICATION_LOG() (Logger::getInstance() << logger::newlog)

GT_VERIFICATION_NAMESPACE_END
