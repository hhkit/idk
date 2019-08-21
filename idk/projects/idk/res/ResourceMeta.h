#pragma once
#include <res/Guid.h>
#include <reflect/reflect.h>

namespace idk
{
	class ResourceMeta
	{
	public:
		Guid guid;

		virtual reflect::dynamic reflect() = 0;
		virtual ~ResourceMeta() = default;
	};
}