#pragma once
#include <core/Component.h>
#include <network/network.h>

namespace idk
{
	class ElectronView
		: public Component<ElectronView>
	{
	public:
		static constexpr int SERVER = -1;

		NetworkID network_id {};
		int owner{ SERVER };
		vector<GenericHandle> observed_components;
	};
}