//===-- Unittests for fstat -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/fcntl/open.h"
#include "src/sys/stat/fstat.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include "hdr/fcntl_macros.h"
#include <sys/stat.h>

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcFStatTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcFStatTest, CreatAndReadMode) {
  // The test file is initially writable. We open it for writing and ensure
  // that it indeed can be opened for writing. Next, we close the file and
  // make it readonly using chmod. We test that chmod actually succeeded by
  // trying to open the file for writing and failing.
  constexpr const char *TEST_FILE = "testdata/fstat.test";

  int fd = LIBC_NAMESPACE::open(TEST_FILE, O_CREAT | O_WRONLY, S_IRWXU);
  ASSERT_GT(fd, 0);
  ASSERT_ERRNO_SUCCESS();

  struct stat statbuf;
  ASSERT_THAT(LIBC_NAMESPACE::fstat(fd, &statbuf), Succeeds(0));

  ASSERT_EQ(int(statbuf.st_mode), int(S_IRWXU | S_IFREG));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(TEST_FILE), Succeeds(0));
}

TEST_F(LlvmLibcFStatTest, NonExistentFile) {
  struct stat statbuf;
  ASSERT_THAT(LIBC_NAMESPACE::fstat(-1, &statbuf), Fails(EBADF));
}
