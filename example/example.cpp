#include <iostream>

#include "memory.h"

using namespace std;

struct TestStruct {};

int main()
{
	Cgc::SharedPtr<TestStruct> sptr;

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
