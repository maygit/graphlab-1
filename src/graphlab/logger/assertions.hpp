// Copyright (c) 2005, Google Inc.
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// ---
// This file contains #include information about logging-related stuff.
// Pretty much everybody needs to #include this file so that they can
// log various happenings.
//
#ifndef _ASSERTIONS_H_
#define _ASSERTIONS_H_

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>    // for write()
#endif
#include <string.h>    // for strlen(), strcmp()
#include <assert.h>
#include <errno.h>     // for errno
#include <sstream>
#include <cassert>
#include <graphlab/logger/logger.hpp>

#include <boost/typeof/typeof.hpp>
// On some systems (like freebsd), we can't call write() at all in a
// global constructor, perhaps because errno hasn't been set up.
// Calling the write syscall is safer (it doesn't set errno), so we
// prefer that.  Note we don't care about errno for logging: we just
// do logging on a best-effort basis.
#define WRITE_TO_STDERR(buf, len) (logbuf(LOG_FATAL, buf, len))

// CHECK dies with a fatal error if condition is not true.  It is *not*
// controlled by NDEBUG, so the check will be executed regardless of
// compilation mode.  Therefore, it is safe to do things like:
//    CHECK(fp->Write(x) == 4)
// Note we use write instead of printf/puts to avoid the risk we'll
// call malloc().
#define CHECK(condition)                                                \
  do {                                                                  \
    if (!(condition)) {                                                 \
      WRITE_TO_STDERR("Check failed: " #condition "\n",                 \
                      sizeof("Check failed: " #condition "\n")-1);      \
      sleep(1);assert(false);                                            \
    }                                                                   \
  } while(0)


// This prints errno as well.  Note we use write instead of printf/puts to
// avoid the risk we'll call malloc().
#define PCHECK(condition)                                               \
  do {                                                                  \
    if (!(condition)) {                                                 \
      const int err_no = errno;                                         \
      logstream(LOG_FATAL) << "Check failed: " << #condition << ": "    \
                           << strerror(err_no) << "\n";                 \
      sleep(1);assert(false);                                           \
    }                                                                   \
  } while(0)

// Helper macro for binary operators; prints the two values on error
// Don't use this macro directly in your code, use CHECK_EQ et al below

// WARNING: These don't compile correctly if one of the arguments is a pointer
// and the other is NULL. To work around this, simply static_cast NULL to the
// type of the desired pointer.

// TODO(jandrews): Also print the values in case of failure.  Requires some
// sort of type-sensitive ToString() function.
#define CHECK_OP(op, val1, val2)                                        \
  do { /*                                                                 \
    typeof(val1) v1 = val1;                                             \
    typeof(val2) v2 = (typeof(val2))val2;                               \
    if (!((v1) op (typeof(val1))(v2))) {                                \
      logstream(LOG_FATAL) << "Check failed: " << #val1 << #op << #val2 \
                           << "  [" << v1 << #op << v2 << "]\n";        \
      sleep(1);assert(false);                                           \
    }    */                                                               \
  } while(0)

#define CHECK_EQ(val1, val2) CHECK_OP(==, val1, val2)
#define CHECK_NE(val1, val2) CHECK_OP(!=, val1, val2)
#define CHECK_LE(val1, val2) CHECK_OP(<=, val1, val2)
#define CHECK_LT(val1, val2) CHECK_OP(< , val1, val2)
#define CHECK_GE(val1, val2) CHECK_OP(>=, val1, val2)
#define CHECK_GT(val1, val2) CHECK_OP(> , val1, val2)

// Synonyms for CHECK_* that are used in some unittests.
#define EXPECT_EQ(val1, val2) CHECK_EQ(val1, val2)
#define EXPECT_NE(val1, val2) CHECK_NE(val1, val2)
#define EXPECT_LE(val1, val2) CHECK_LE(val1, val2)
#define EXPECT_LT(val1, val2) CHECK_LT(val1, val2)
#define EXPECT_GE(val1, val2) CHECK_GE(val1, val2)
#define EXPECT_GT(val1, val2) CHECK_GT(val1, val2)
#define ASSERT_EQ(val1, val2) EXPECT_EQ(val1, val2)
#define ASSERT_NE(val1, val2) EXPECT_NE(val1, val2)
#define ASSERT_LE(val1, val2) EXPECT_LE(val1, val2)
#define ASSERT_LT(val1, val2) EXPECT_LT(val1, val2)
#define ASSERT_GE(val1, val2) EXPECT_GE(val1, val2)
#define ASSERT_GT(val1, val2) EXPECT_GT(val1, val2)
// As are these variants.
#define EXPECT_TRUE(cond)     CHECK(cond)
#define EXPECT_FALSE(cond)    CHECK(!(cond))
#define EXPECT_STREQ(a, b)    CHECK(strcmp(a, b) == 0)
#define ASSERT_TRUE(cond)     EXPECT_TRUE(cond)
#define ASSERT_FALSE(cond)    EXPECT_FALSE(cond)
#define ASSERT_STREQ(a, b)    EXPECT_STREQ(a, b)

#define logger(lvl,fmt,...)                 \
    (global_logger()._log(lvl,__FILE__,     \
                        __func__ ,__LINE__,fmt,##__VA_ARGS__))


#define ASSERT_MSG(condition, fmt, ...)                                 \
  do {                                                                  \
    if (!(condition)) {                                                 \
      logstream(LOG_FATAL) << "Check failed: " << #condition << ":\n";  \
      logger(LOG_FATAL, fmt, ##__VA_ARGS__);                            \
      sleep(1);assert(false);                                            \
    }                                                                   \
  } while(0)

// Used for (libc) functions that return -1 and set errno
#define CHECK_ERR(invocation)  PCHECK((invocation) != -1)

// A few more checks that only happen in debug mode
#ifdef NDEBUG
#define DCHECK_EQ(val1, val2)
#define DCHECK_NE(val1, val2)
#define DCHECK_LE(val1, val2)
#define DCHECK_LT(val1, val2)
#define DCHECK_GE(val1, val2)
#define DCHECK_GT(val1, val2)
#define DASSERT_TRUE(cond)
#define DASSERT_FALSE(cond)
#define DASSERT_MSG(condition, fmt, ...)

#else
#define DCHECK_EQ(val1, val2)  CHECK_EQ(val1, val2)
#define DCHECK_NE(val1, val2)  CHECK_NE(val1, val2)
#define DCHECK_LE(val1, val2)  CHECK_LE(val1, val2)
#define DCHECK_LT(val1, val2)  CHECK_LT(val1, val2)
#define DCHECK_GE(val1, val2)  CHECK_GE(val1, val2)
#define DCHECK_GT(val1, val2)  CHECK_GT(val1, val2)
#define DASSERT_TRUE(cond)     ASSERT_TRUE(cond)
#define DASSERT_FALSE(cond)    ASSERT_FALSE(cond)
#define DASSERT_MSG(condition, fmt, ...)                                 \
  do {                                                                  \
    if (!(condition)) {                                                 \
      logstream(LOG_FATAL) << "Check failed: " << #condition << ":\n";  \
      logger(LOG_FATAL, fmt, ##__VA_ARGS__);                            \
     sleep(1);assert(false);                                            \
    }                                                                   \
  } while(0)

#endif


#ifdef ERROR
#undef ERROR      // may conflict with ERROR macro on windows
#endif

#endif // _LOGGING_H_
