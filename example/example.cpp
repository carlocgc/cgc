#include <iostream>
#include "memory.h"


const char* GetString()
{
	char buffer[128];

	sprintf_s(buffer, 128, "%s%s", "Hello", "World");
	
	return buffer;
}

int main()
{
	const char* string = GetString();

	std::cout << string << std::endl;

	return 0;
}
