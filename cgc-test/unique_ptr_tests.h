#pragma once
#include "gtest/gtest.h"
#include "memory.h"
#include "test_contexts.h"

TEST(UniquePtr, DefaultConstruction)
{
	cgc::unique_ptr<test_struct> ptr{};
	ASSERT_FALSE(ptr) << "unique_ptr not null when default constructed";
}

TEST(UniquePtr, PointerConstruction)
{
	cgc::unique_ptr<test_struct> ptr{ new test_struct{} };
	ASSERT_TRUE(ptr) << "unique_ptr not valid when constructed with a pointer";
}

TEST(UniquePtr, MoveConstruction)
{
	cgc::unique_ptr<test_struct> ptr{};
	EXPECT_FALSE(ptr) << "unique_ptr not null when default constructed";

	ptr = cgc::unique_ptr<test_struct>{ new test_struct{} };
	ASSERT_TRUE(ptr) << "unique_ptr not valid when move constructed";
}

TEST(UniquePtr, AreEqual)
{
	cgc::unique_ptr<test_struct> ptr_a{ new test_struct{} };
	EXPECT_TRUE(ptr_a) << "unique_ptr not valid when constructed with a pointer";

	ASSERT_TRUE(ptr_a == ptr_a);
}

TEST(UniquePtr, AreNotEqual)
{	
	cgc::unique_ptr<test_struct> ptr_a{ new test_struct{} };
	EXPECT_TRUE(ptr_a) << "unique_ptr not valid when constructed with a pointer";

	cgc::unique_ptr<test_struct> ptr_b{ new test_struct{} };
	EXPECT_TRUE(ptr_b) << "unique_ptr not valid when constructed with a pointer";

	ASSERT_TRUE(ptr_a != ptr_b);
}