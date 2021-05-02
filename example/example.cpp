#include "memory.h"

struct test_struct
{
	
};

int main()
{
	cgc::unique_ptr<test_struct> ptr{ new test_struct{} };

	cgc::unique_ptr<test_struct> ptr_b{ cgc::unique_ptr<test_struct>{ new test_struct{} } };
	
	return 0;	
}
