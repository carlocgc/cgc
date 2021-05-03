#include <iostream>

#include "memory.h"

using namespace std;

struct TestStruct {};

int main()
{
	Cgc::SharedPtr<TestStruct> sptr_a{new TestStruct{}};

	auto wptr = sptr_a.GetWeak();

	if (auto sptr = wptr.TryLock())
	{
		cout << "wptr is valid" << endl;
		// Do something with sptr here while lifetime is extended
	}
	else
	{
		cout << "wptr is invalid" << endl;
	}
	
	return 0;	
}
