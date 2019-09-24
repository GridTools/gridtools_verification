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

#include "include_boost_format.h"
#include <chrono>
#include <cstdio>
#include <ostream>
#include "../common.h"

namespace gt_verification {

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
     * The logger can be enabled by passing the command_line option --log (-l) or by setting the
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
    enum class logger_action {
        nop,   /**< Do nothing */
        flush, /**< Flush log stream */
        endl,  /**< Insert a newline character and flush log stream */
        newlog /**< Indicate if this is the first formatting operation on this log */
    };

    /**
     * @brief Trivial Logger
     * @ingroup Logger
     */
    class logger : private boost::noncopyable /* singleton */
    {
        logger();

      public:
        /**
         * @brief Return the instance of the Logger
         */
        static logger &getInstance();

        /**
         * @brief Inserts data into the logging stream
         */
        template < class ValueType >
        logger &operator<<(const ValueType &value) noexcept {
            if (enable_)
                this->logImpl(value);
            return (*this);
        }

        /**
         * @brief Inserts a special @c logger character to control the behaviour of the stream
         */
        logger &operator<<(const logger_action &loggerAction) noexcept {
            if (enable_) {
                logger_action loggerSwitch =
                    (loggerAction == logger_action::newlog && !flushed_) ? logger_action::endl : loggerAction;

                switch (loggerSwitch) {
                case logger_action::endl:
                    outStream_.put(outStream_.widen('\n'));
                case logger_action::flush:
                    outStream_.flush();
                    flushed_ = true;
                case logger_action::nop:
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

        static logger *instance_;
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
#define VERIFICATION_LOG() (logger::getInstance() << logger_action::newlog)
}
