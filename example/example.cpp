#include "memory.h"

struct test_struct
{
	
};

int main()
{
	Cgc::UniquePtr<test_struct> ptr{ new test_struct{} };

	Cgc::UniquePtr<test_struct> ptr_b{ Cgc::UniquePtr<test_struct>{ new test_struct{} } };
	
	return 0;	
}
