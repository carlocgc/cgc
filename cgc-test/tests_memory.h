#pragma once
#include "pch.h"
#include "memory.h"

struct test_struct
{
};

TEST(SharedPtr, IsNullConstructed)
{
	const cgc::shared_ptr<test_struct> ptr{};
	ASSERT_FALSE(ptr);
}

TEST(SharedPtr, NotNullConstructed)
{
	const cgc::shared_ptr<test_struct> ptr{ new test_struct{} };
	ASSERT_TRUE(ptr);
}