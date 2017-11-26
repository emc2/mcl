/* Copyright (c) 2017, Eric L. McCorkle. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _MCL_H_
#define _MCL_H_

/*!
 * \brief Macro-controlled logging (MCL) API.
 *
 * The Macro-Controlled Logging (MCL) API is a lightweight logging API
 * which uses the C preprocessor, allowing it to operate with minimal
 * overhead.  Logging statements can be disabled,
 * dynamically-controlled, or hardwired through the use of
 * preprocessor macros.
 *
 * To use MCL, you will need to define logging systems.  This is done
 * with the DEFINE_LOGGING_SYSTEM macro:
 * \code
 * DEFINE_LOG_SYSTEM(example, LOG_LVL_ERROR);
 * \endcode
 *
 * This will generate the necessary definitions for a logging system
 * named "example".  To allow other files to access this logging
 * system, generate a declaration in a header file with the
 * DECLARE_LOG_SYSTEM macro:
 * \code
 * DECLARE_LOG_SYSTEM(example);
 * \endcode
 *
 * The log level of a logging system can be changed dynamically if
 * DYNAMIC_LOG_LVL is enabled (as it is by default) using
 * GET_LOG_SYSTEM_LVL to query the logging level for the system and
 * SET_LOG_SYSTEM_LVL to set it.  If DYNAMIC_LOG_LVL is disabled,
 * these operations are eliminated at compile time.
 *
 * The minimum and maximum dynamic logging levels are controlled by
 * DYNAMIC_LOG_LVL_MIN and DYNAMIC_LOG_LVL_MAX respectively.
 * SET_LOG_SYSTEM_LVL will clamp the logging level to lie between
 * DYNAMIC_LOG_LVL_MIN and DYNAMIC_LOG_LVL_MAX.  The default for
 * DYNAMIC_LOG_LVL_MIN is LOG_LVL_ERROR and for DYNAMIC_LOG_LVL_MAX,
 * it is LOG_LVL_VERBOSE.
 *
 * There is a logging macro for each log level (ex: LOG_FATAL,
 * LOG_ERROR, etc.) which will generate log messages at that level.
 * Any logging statement at a level below DYNAMIC_LOG_LVL_MIN will be
 * hardwired into place, while any logging statement at a level above
 * DYNAMIC_LOG_LVL_MAX will be deleted at compile time.
 *
 * Lastly, the logging action is defined by LOG_ACTION.  The default
 * prints any logging message at level LOG_LVL_WARN or lower to
 * stderr, and all others to stdout.
 *
 * \file mcl.h
 */

/*!
 * \brief Whether or not to allow the log level to be changed at
 * runtime.
 */
#ifndef DYNAMIC_LOG_LVL
#define DYNAMIC_LOG_LVL 1
#endif

typedef char __log_lvl_t;

/*!
 * \brief Log level for fatal conditions.
 */
#define LOG_LVL_FATAL 0

/*!
 * \brief Log level for non-fatal errors.
 */
#define LOG_LVL_ERROR 1

/*!
 * \brief Log level for likely problems.
 */
#define LOG_LVL_WARN 2

/*!
 * \brief Log level for terse messages during normal operation.
 */
#define LOG_LVL_MSG 3

/*!
 * \brief Log level for progress messages during normal operation.
 */
#define LOG_LVL_INFO 4

/*!
 * \brief Log level for verbose messages during normal operation.
 */
#define LOG_LVL_VERBOSE 5

/*!
 * \brief Log level for debug messages.
 */
#define LOG_LVL_DEBUG 6

/*!
 * \brief Log level for trace debugging.
 */
#define LOG_LVL_TRACE 7

/*!
 * \brief Ignore all log messages above the hardwire threshold.
 */
#define LOG_LVL_NONE (-1)

/*!
 * \brief Print all log messages below the omission threshold.
 */
#define LOG_LVL_ALL 0x7f

/*!
 * \brief Highest level of logging that can be dynamically set.
 *
 * Logging statements above this level will be deleted at compile time
 * (and thus will have no runtime cost).  By default, this is set to
 * LOG_LVL_VERBOSE.  Set this to LOG_LVL_ALL to avoid deleting logging
 * statements at compile time.
 */
#ifndef DYNAMIC_LOG_LVL_MAX
#define DYNAMIC_LOG_LVL_MAX LOG_LVL_VERBOSE
#endif

/*!
 * \brief Lowest level of logging that can be dynamically set.
 *
 * Logging statements below this level will be hardwired into place at
 * compile time (and thus cannot be silenced).  By default, this is
 * set to LOG_LVL_WARN.
 */
#ifndef DYNAMIC_LOG_LVL_MIN
#define DYNAMIC_LOG_LVL_MIN LOG_LVL_WARN
#endif

#if DYNAMIC_LOG_LVL_MIN > DYNAMIC_LOG_LVL_MAX
#error "DYNAMIC_LOG_LVL_MIN cannot be greater than DYNAMIC_LOG_LVL_MAX"
#endif

#ifndef LOG_ACTION
#include <stdio.h>
#endif

/*!
 * \brief Action to use for logging.
 *
 * \param lvl The logging level.
 * \param args The printf-style arguments.
 */
#ifndef LOG_ACTION
#define LOG_ACTION(lvl, args)  \
  {                            \
    switch(lvl) {              \
      case LOG_LVL_FATAL:      \
      case LOG_LVL_ERROR:      \
      case LOG_LVL_WARN:       \
        fprintf(stderr, args); \
        break;                 \
      default:                 \
        fprintf(stdout, args); \
        break;                 \
    }                          \
  }
#endif

/*!
 * \brief Log a message at the fatal level.
 * \param name The name of the logging system to use.
 * \param args The printf-style arguments.
 */
#if DYNAMIC_LOG_LVL_MAX >= LOG_LVL_FATAL
#if DYNAMIC_LOG_LVL && (DYNAMIC_LOG_LVL_MIN <= LOG_LVL_FATAL)
#define LOG_FATAL(name, args...)                  \
  {                                               \
    if(__ ## name ## _log_lvl >= LOG_LVL_FATAL) { \
      LOG_ACTION(LOG_LVL_FATAL, args);            \
    }                                             \
  }
#else
#define LOG_FATAL(name, args...) { LOG_ACTION(LOG_LVL_FATAL, args); }
#endif
#else
#define LOG_FATAL(name, args...)
#endif

/*!
 * \brief Log a message at the error level.
 * \param name The name of the logging system to use.
 * \param args The printf-style arguments.
 */
#if DYNAMIC_LOG_LVL_MAX >= LOG_LVL_ERROR
#if DYNAMIC_LOG_LVL && (DYNAMIC_LOG_LVL_MIN <= LOG_LVL_ERROR)
#define LOG_ERROR(name, args...)                  \
  {                                               \
    if(__ ## name ## _log_lvl >= LOG_LVL_ERROR) { \
      LOG_ACTION(LOG_LVL_ERROR, args);            \
    }                                             \
  }
#else
#define LOG_ERROR(name, args...) { LOG_ACTION(LOG_LVL_ERROR, args); }
#endif
#else
#define LOG_ERROR(name, args...)
#endif

/*!
 * \brief Log a message at the warning level.
 * \param name The name of the logging system to use.
 * \param args The printf-style arguments.
 */
#if DYNAMIC_LOG_LVL_MAX >= LOG_LVL_WARN
#if DYNAMIC_LOG_LVL && (DYNAMIC_LOG_LVL_MIN <= LOG_LVL_WARN)
#define LOG_WARN(name, args...)                  \
  {                                              \
    if(__ ## name ## _log_lvl >= LOG_LVL_WARN) { \
      LOG_ACTION(LOG_LVL_WARN, args);            \
    }                                            \
  }
#else
#define LOG_WARN(name, args...) { LOG_ACTION(LOG_LVL_WARN, args); }
#endif
#else
#define LOG_WARN(name, args...)
#endif

/*!
 * \brief Log a message at the message level.
 * \param name The name of the logging system to use.
 * \param args The printf-style arguments.
 */
#if DYNAMIC_LOG_LVL_MAX >= LOG_LVL_MSG
#if DYNAMIC_LOG_LVL && (DYNAMIC_LOG_LVL_MIN <= LOG_LVL_MSG)
#define LOG_MSG(name, args...)                  \
  {                                             \
    if(__ ## name ## _log_lvl >= LOG_LVL_MSG) { \
      LOG_ACTION(LOG_LVL_MSG, args);            \
    }                                           \
  }
#else
#define LOG_MSG(name, args...) { LOG_ACTION(LOG_LVL_MSG, args); }
#endif
#else
#define LOG_MSG(name, args...)
#endif

/*!
 * \brief Log a message at the info level.
 * \param name The name of the logging system to use.
 * \param args The printf-style arguments.
 */
#if DYNAMIC_LOG_LVL_MAX >= LOG_LVL_INFO
#if DYNAMIC_LOG_LVL && (DYNAMIC_LOG_LVL_MIN <= LOG_LVL_INFO)
#define LOG_INFO(name, args...)                  \
  {                                              \
    if(__ ## name ## _log_lvl >= LOG_LVL_INFO) { \
      LOG_ACTION(LOG_LVL_INFO, args);            \
    }                                            \
  }
#else
#define LOG_INFO(name, args...) { LOG_ACTION(LOG_LVL_INFO, args); }
#endif
#else
#define LOG_INFO(name, args...)
#endif

/*!
 * \brief Log a message at the verbose level.
 * \param name The name of the logging system to use.
 * \param args The printf-style arguments.
 */
#if DYNAMIC_LOG_LVL_MAX >= LOG_LVL_VERBOSE
#if DYNAMIC_LOG_LVL && (DYNAMIC_LOG_LVL_MIN <= LOG_LVL_VERBOSE)
#define LOG_VERBOSE(name, args...)                  \
  {                                                 \
    if(__ ## name ## _log_lvl >= LOG_LVL_VERBOSE) { \
      LOG_ACTION(LOG_LVL_VERBOSE, args);            \
    }                                               \
  }
#else
#define LOG_VERBOSE(name, args...) { LOG_ACTION(LOG_LVL_VERBOSE, args); }
#endif
#else
#define LOG_VERBOSE(name, args...)
#endif

/*!
 * \brief Log a message at the debug level.
 * \param name The name of the logging system to use.
 * \param args The printf-style arguments.
 */
#if DYNAMIC_LOG_LVL_MAX >= LOG_LVL_DEBUG
#if DYNAMIC_LOG_LVL && (DYNAMIC_LOG_LVL_MIN <= LOG_LVL_DEBUG)
#define LOG_DEBUG(name, args...)                  \
  {                                               \
    if(__ ## name ## _log_lvl >= LOG_LVL_DEBUG) { \
      LOG_ACTION(LOG_LVL_DEBUG, args);            \
    }                                             \
  }
#else
#define LOG_DEBUG(name, args...) { LOG_ACTION(LOG_LVL_DEBUG, args); }
#endif
#else
#define LOG_DEBUG(name, args...)
#endif

/*!
 * \brief Log a message at the debug level.
 * \param name The name of the logging system to use.
 * \param args The printf-style arguments.
 */
#if DYNAMIC_LOG_LVL_MAX >= LOG_LVL_TRACE
#if DYNAMIC_LOG_LVL && (DYNAMIC_LOG_LVL_MIN <= LOG_LVL_TRACE)
#define LOG_TRACE(name, args...)                  \
  {                                               \
    if(__ ## name ## _log_lvl >= LOG_LVL_TRACE) { \
      LOG_ACTION(LOG_LVL_TRACE, args);            \
    }                                             \
  }
#else
#define LOG_TRACE(name, args...) { LOG_ACTION(LOG_LVL_TRACE, args); }
#endif
#else
#define LOG_TRACE(name, args...)
#endif

/*!
 * \brief Constrain a potential log level to be within the hard thresholds.
 */
#define __LOG_LVL_CLAMP(lvl)                                 \
  (lvl > DYNAMIC_LOG_LVL_MAX ? DYNAMIC_LOG_LVL_MAX :         \
    (lvl < DYNAMIC_LOG_LVL_MIN ? DYNAMIC_LOG_LVL_MIN : lvl))

/*!
 * \brief Declare a logging system, but don't define it.
 *
 * \param name The name of the logging system.
 * \see DEFINE_LOG_SYSTEM
 */
#if DYNAMIC_LOG_LVL
#define DECLARE_LOG_SYSTEM(name)             \
  extern __log_lvl_t __ ## name ## _log_lvl;
#else
#define DECLARE_LOG_SYSTEM(name)
#endif

/*!
 * \brief Define a logging system.
 *
 * \param name The name of the logging system.
 * \see DECLARE_LOG_SYSTEM.
 */
#if DYNAMIC_LOG_LVL
#define DEFINE_LOG_SYSTEM(name, lvl)                         \
  __log_lvl_t __ ## name ## _log_lvl = __LOG_LVL_CLAMP(lvl);
#else
#define DEFINE_LOG_SYSTEM(name, lvl)
#endif

/*!
 * \brief Get the logging level of a logging system.
 *
 * \param name The name of the systems whose level to get.
 */
#if DYNAMIC_LOG_LVL
#define GET_LOG_SYSTEM_LVL(name) (__ ## name ## _log_lvl)
#else
#define GET_LOG_SYSTEM_LVL(name) DYNAMIC_LOG_LVL_MAX
#endif

/*!
 * \brief Set the logging level for a logging system.
 *
 * \param name The name of the logging system.
 * \param lvl The new level for the logging system.
 */
#if DYNAMIC_LOG_LVL
#define SET_LOG_SYSTEM_LVL(name, lvl)             \
  (__ ## name ## _log_lvl = __LOG_LVL_CLAMP(lvl))
#else
#define SET_LOG_SYSTEM_LVL(name, lvl)
#endif

#endif
