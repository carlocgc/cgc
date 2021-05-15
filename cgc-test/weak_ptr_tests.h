#pragma once

#include "gtest/gtest.h"
#include "memory.h"
#include "test_contexts.h"

TEST(WeakPtr, DefaultConstruction)
{
	cgc::weak_ptr<test_struct> wptr{};
	ASSERT_FALSE(wptr) << "weak_ptr not null when default constructed";
}

TEST(WeakPtr, ConstructionWithValidSharedPtrIsValid)
{
	cgc::shared_ptr<test_struct> sptr{ new test_struct{} };
	EXPECT_TRUE(sptr) << "shared_ptr not valid when constructed with a pointer";

	cgc::weak_ptr<test_struct> wptr{ sptr };
	ASSERT_TRUE(wptr);
}

TEST(WeakPtr, ConstructionWithNullSharedPtrIsNull)
{
	cgc::shared_ptr<test_struct> sptr{};
	EXPECT_FALSE(sptr);

	cgc::weak_ptr<test_struct> wptr{ sptr };
	ASSERT_FALSE(wptr);
}

TEST(WeakPtr, MoveConstruction)
{
	cgc::shared_ptr<test_struct> sptr{ new test_struct{} };
	EXPECT_TRUE(sptr);

	cgc::weak_ptr<test_struct> wptr = cgc::weak_ptr<test_struct>{ sptr };
	EXPECT_TRUE(wptr);
}

TEST(WeakPtr, CopyConstruction)
{
	cgc::shared_ptr<test_struct> sptr{ new test_struct{}};
	EXPECT_TRUE(sptr);

	cgc::weak_ptr<test_struct> wptr_a = sptr.get_weak();	
	EXPECT_TRUE(wptr_a);

	cgc::weak_ptr<test_struct> wptr_b = wptr_a;
	ASSERT_TRUE(wptr_b);
}

TEST(WeakPtr, WeakPtrDoesExpire)
{
	cgc::weak_ptr<test_struct> wptr;
	EXPECT_FALSE(wptr.try_lock());

	{
		cgc::shared_ptr<test_struct> sptr{ new test_struct{} };
		wptr = sptr.get_weak();
		EXPECT_TRUE(wptr.try_lock());
	}

	ASSERT_FALSE(wptr.try_lock());
}

TEST(WeakPtr, AllWeakPtrsDoExpire)
{
	const int amount = 10;
	std::vector < cgc::weak_ptr<test_struct>> weak_ptrs;

	{
		cgc::shared_ptr<test_struct> sptr{ new test_struct{} };

		for (auto i = 0; i < amount; ++i)
		{
			weak_ptrs.push_back(sptr.get_weak());
			EXPECT_TRUE(weak_ptrs[i].try_lock());
		}
	}

	for (auto i = 0; i < amount; ++i)
	{
		ASSERT_FALSE(weak_ptrs[i].try_lock());
	}
}

TEST(WeakPtr, AreEqual)
{
	cgc::shared_ptr<test_struct> sptr_a{ new test_struct{} };
	EXPECT_TRUE(sptr_a) << "shared_ptr not valid when constructed with a pointer";

	cgc::weak_ptr<test_struct> wptr_a = sptr_a.get_weak();
	EXPECT_TRUE(wptr_a);

	cgc::weak_ptr<test_struct> wptr_b = sptr_a.get_weak();
	EXPECT_TRUE(wptr_b);

	ASSERT_TRUE(wptr_a == wptr_b);
}

TEST(WeakPtr, AreEqualAfterCopy)
{
	cgc::shared_ptr<test_struct> sptr_a{ new test_struct{} };
	EXPECT_TRUE(sptr_a) << "shared_ptr not valid when constructed with a pointer";

	cgc::weak_ptr<test_struct> wptr_a = sptr_a.get_weak();
	EXPECT_TRUE(wptr_a);

	cgc::weak_ptr<test_struct> wptr_b = wptr_a;
	EXPECT_TRUE(wptr_b);

	ASSERT_TRUE(wptr_a == wptr_b);
}

TEST(WeakPtr, AreNotEqual)
{
	cgc::shared_ptr<test_struct> sptr_a{ new test_struct{} };
	EXPECT_TRUE(sptr_a) << "shared_ptr not valid when constructed with a pointer";

	cgc::shared_ptr<test_struct> sptr_b{ new test_struct{} };
	EXPECT_TRUE(sptr_b) << "shared_ptr not valid when constructed with a pointer";

	cgc::weak_ptr<test_struct> wptr_a = sptr_a.get_weak();
	EXPECT_TRUE(wptr_a);

	cgc::weak_ptr<test_struct> wptr_b = sptr_b.get_weak();
	EXPECT_TRUE(wptr_a);

	ASSERT_TRUE(wptr_a != wptr_b);
}