#pragma once
#include <yojimbo/yojimbo.h>
#include <core/ISystem.h>

namespace idk
{

	class NetworkSystem
		: public ISystem
	{

	public:
		void InstantiateServer();
	private:
		yojimbo::Address my_addr;

		void Init() override;
		void Shutdown() override;
	};
}