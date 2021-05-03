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

TEST(WeakPtr, WeakPtrDoesExpire)
{
	Cgc::WeakPtr<TestStruct> wptr;
	EXPECT_FALSE(wptr.TryLock());

	{
		Cgc::SharedPtr<TestStruct> sptr{ new TestStruct{} };
		wptr = sptr.GetWeak();
		EXPECT_TRUE(wptr.TryLock());
	}

	ASSERT_FALSE(wptr.TryLock());
}

TEST(WeakPtr, AllWeakPtrsDoExpire)
{
	const int amount = 10;
	std::vector < Cgc::WeakPtr<TestStruct>> weak_ptrs;

	{
		Cgc::SharedPtr<TestStruct> sptr{ new TestStruct{} };

		for (auto i = 0; i < amount; ++i)
		{
			weak_ptrs.push_back(sptr.GetWeak());
			EXPECT_TRUE(weak_ptrs[i].TryLock());
		}
	}

	for (auto i = 0; i < amount; ++i)
	{
		ASSERT_FALSE(weak_ptrs[i].TryLock());
	}	
}