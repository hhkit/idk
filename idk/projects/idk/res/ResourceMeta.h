#pragma once
#include <res/Guid.h>

namespace idk
{
	namespace reflect
	{
		class dynamic;
	}

	class ResourceMeta
	{
	public:
		Guid guid;

		virtual reflect::dynamic reflect() = 0;
		virtual ~ResourceMeta() = default;
	};
}