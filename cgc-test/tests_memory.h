#pragma once
#include "pch.h"
#include "memory.h"

struct test_struct
{
};

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
