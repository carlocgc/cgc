#include <iostream>

#include "memory.h"

using namespace std;

struct test_struct
{
	
};

int main()
{
	Cgc::SharedPtr<test_struct> sptr;

	auto wptr = sptr.GetWeak();

	if (wptr.TryLock())
	{
		cout << "wptr is valid" << endl;
	}
	else
	{
		cout << "wptr is invalid" << endl;
	}
	
	return 0;	
}
