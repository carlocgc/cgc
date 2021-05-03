#pragma once
#include "pch.h"
#include "memory.h"

struct test_struct
{
};

TEST(SharedPtr, DefaultConstruction)
{
	const Cgc::SharedPtr<test_struct> ptr{};
	ASSERT_FALSE(ptr) << "SharedPtr not null when default constructed";
}

TEST(SharedPtr, PointerConstruction)
{
	const Cgc::SharedPtr<test_struct> ptr{ new test_struct{} };
	ASSERT_TRUE(ptr) << "SharedPtr not valid when constructed with a pointer";
}

TEST(SharedPtr, MoveConstruction)
{
	Cgc::SharedPtr<test_struct> ptr;
	EXPECT_FALSE(ptr) << "SharedPtr not null when default constructed";

	ptr = Cgc::SharedPtr<test_struct>{ new test_struct{} };
	ASSERT_TRUE(ptr) << "SharedPtr not valid when move constructed";
}

TEST(SharedPtr, CopyConstruction)
{
	Cgc::SharedPtr<test_struct> ptr{ new test_struct{} };
	EXPECT_TRUE(ptr);

	Cgc::SharedPtr<test_struct> ptr_b{ ptr };
	ASSERT_TRUE(ptr_b) << "SharedPtr not valid when copy constructed";	
}
