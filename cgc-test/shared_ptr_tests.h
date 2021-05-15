#pragma once

#include "gtest/gtest.h"
#include "memory.h"
#include "test_contexts.h"

TEST(SharedPtr, DefaultConstruction)
{
	const cgc::shared_ptr<test_struct> ptr{};
	ASSERT_FALSE(ptr) << "shared_ptr not null when default constructed";
}
TEST(SharedPtr, PointerConstruction)
{
	const cgc::shared_ptr<test_struct> ptr{ new test_struct{} };
	ASSERT_TRUE(ptr) << "shared_ptr not valid when constructed with a pointer";
}

TEST(SharedPtr, MoveConstruction)
{
	cgc::shared_ptr<test_struct> ptr;
	EXPECT_FALSE(ptr) << "shared_ptr not null when default constructed";

	ptr = cgc::shared_ptr<test_struct>{ new test_struct{} };
	ASSERT_TRUE(ptr) << "shared_ptr not valid when move constructed";
}

TEST(SharedPtr, CopyConstruction)
{
	cgc::shared_ptr<test_struct> ptr{ new test_struct{} };
	EXPECT_TRUE(ptr);

	cgc::shared_ptr<test_struct> ptr_b{ ptr };
	ASSERT_TRUE(ptr_b) << "shared_ptr not valid when copy constructed";	
}

TEST(SharedPtr, GetWeakIsNull)
{
	cgc::shared_ptr<test_struct> sptr{};

	auto wptr = sptr.get_weak();

	ASSERT_FALSE(wptr.try_lock());
}

TEST(SharedPtr, GetWeakNotNull)
{
	cgc::shared_ptr<test_struct> sptr{ new test_struct() };

	auto wptr = sptr.get_weak();

	ASSERT_TRUE(wptr.try_lock());
}

TEST(SharedPtr, AreEqual)
{
	cgc::shared_ptr<test_struct> sptr_a{ new test_struct{} };
	EXPECT_TRUE(sptr_a) << "shared_ptr not valid when constructed with a pointer";	

	ASSERT_TRUE(sptr_a == sptr_a);
}

TEST(SharedPtr, AreEqualAfterCopy)
{
	cgc::shared_ptr<test_struct> sptr_a{ new test_struct{} };
	EXPECT_TRUE(sptr_a) << "shared_ptr not valid when constructed with a pointer";

	cgc::shared_ptr<test_struct> sptr_b = sptr_a;
	EXPECT_TRUE(sptr_a) << "shared_ptr not valid when constructed with a pointer";

	ASSERT_TRUE(sptr_a == sptr_b);
}

TEST(SharedPtr, AreNotEqual)
{
	cgc::shared_ptr<test_struct> sptr_a{ new test_struct{} };
	EXPECT_TRUE(sptr_a) << "shared_ptr not valid when constructed with a pointer";

	cgc::shared_ptr<test_struct> sptr_b{ new test_struct{} };
	EXPECT_TRUE(sptr_b) << "shared_ptr not valid when constructed with a pointer";

	ASSERT_TRUE(sptr_a != sptr_b);
}
