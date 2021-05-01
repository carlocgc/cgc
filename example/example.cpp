#include <iostream>
#include "memory.h"

struct my_struct
{	
};

int main()
{    
	cgc::shared_ptr<my_struct> ptr_a;

	if (ptr_a)
	{
		std::cout << "Hello World!\n";
	}
	
	{
		cgc::shared_ptr<my_struct> ptr_b(new my_struct());

		ptr_a = ptr_b;
	}

	if (ptr_a)
	{
		std::cout << "Hello World!\n";
	}
}
