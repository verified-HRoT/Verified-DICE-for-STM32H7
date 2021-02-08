/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/common.h"
#include <gtest/gtest.h>

#define DEF_TEST(testName, methodName) TEST(testName, methodName)

#define DEF_TEST_F(testName, methodName) TEST_F(testName, methodName)

#define GT_ASSERT_EQ ASSERT_EQ

#define GT_ASSERT_NE ASSERT_NE

#define GT_ASSERT_TRUE ASSERT_TRUE

#define GT_EXPECT_EQ EXPECT_EQ

#define GT_EXPECT_NE EXPECT_NE

#define GT_EXPECT_TRUE EXPECT_TRUE
