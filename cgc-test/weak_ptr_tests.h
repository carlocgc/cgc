#pragma once

#include "gtest/gtest.h"
#include "memory.h"
#include "test_contexts.h"

TEST(WeakPtr, GetWeakIsNull)
{
	Cgc::SharedPtr<TestStruct> sptr{};

	auto wptr = sptr.GetWeak();

	ASSERT_FALSE(wptr.TryLock());
}

TEST(WeakPtr, GetWeakNotNull)
{
	Cgc::SharedPtr<TestStruct> sptr{ new TestStruct() };

	auto wptr = sptr.GetWeak();

	ASSERT_TRUE(wptr.TryLock());
}