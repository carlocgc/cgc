#pragma once
#include "gtest/gtest.h"
#include "memory.h"
#include "test_contexts.h"

TEST(UniquePtr, DefaultConstruction)
{
	const Cgc::UniquePtr<TestStruct> ptr{};
	ASSERT_FALSE(ptr) << "UniquePtr not null when default constructed";
}

TEST(UniquePtr, PointerConstruction)
{
	const Cgc::UniquePtr<TestStruct> ptr{ new TestStruct{} };
	ASSERT_TRUE(ptr) << "UniquePtr not valid when constructed with a pointer";
}

TEST(UniquePtr, MoveConstruction)
{
	Cgc::UniquePtr<TestStruct> ptr;
	EXPECT_FALSE(ptr) << "UniquePtr not null when default constructed";

	ptr = Cgc::UniquePtr<TestStruct>{ new TestStruct{} };
	ASSERT_TRUE(ptr) << "UniquePtr not valid when move constructed";
}