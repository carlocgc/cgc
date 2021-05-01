#pragma once

namespace cgc
{	
	struct non_copyable
	{
		non_copyable() = default;
		non_copyable(non_copyable const&) = delete;
		non_copyable& operator = (non_copyable const&) = delete;
	};	
}