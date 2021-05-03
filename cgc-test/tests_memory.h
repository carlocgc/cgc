#pragma once
#include "pch.h"
#include "memory.h"

struct TestStruct
{
};

TEST(SharedPtr, DefaultConstruction)
{
	const Cgc::SharedPtr<TestStruct> ptr{};
	ASSERT_FALSE(ptr) << "SharedPtr not null when default constructed";
}

TEST(SharedPtr, PointerConstruction)
{
	const Cgc::SharedPtr<TestStruct> ptr{ new TestStruct{} };
	ASSERT_TRUE(ptr) << "SharedPtr not valid when constructed with a pointer";
}

TEST(SharedPtr, MoveConstruction)
{
	Cgc::SharedPtr<TestStruct> ptr;
	EXPECT_FALSE(ptr) << "SharedPtr not null when default constructed";

	ptr = Cgc::SharedPtr<TestStruct>{ new TestStruct{} };
	ASSERT_TRUE(ptr) << "SharedPtr not valid when move constructed";
}

TEST(SharedPtr, CopyConstruction)
{
	Cgc::SharedPtr<TestStruct> ptr{ new TestStruct{} };
	EXPECT_TRUE(ptr);

	Cgc::SharedPtr<TestStruct> ptr_b{ ptr };
	ASSERT_TRUE(ptr_b) << "SharedPtr not valid when copy constructed";	
}
