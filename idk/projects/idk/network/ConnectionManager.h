#pragma once
#include <yojimbo/yojimbo.h>
#include <idk.h>
#include <core/ISystem.h>

namespace idk
{
	class ConnectionManager
		: public ISystem
	{
	public:

		bool IsHost();
	private:
		void Init();
		void Shutdown();
	};
}