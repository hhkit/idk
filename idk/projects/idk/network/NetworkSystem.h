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
		bool IsHost();
	private:
		std::unique_ptr<class Lobby> lobby;
		//yojimbo::Address my_addr;

		void Init() override;
		void Shutdown() override;
	};
}