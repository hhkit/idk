#pragma once
#include <core/Component.h>
#include <network/network.h>

namespace idk
{
	class ElectronView
		: public Component<ElectronView>
	{
	public:
		NetworkID network_id {};
		bool hack{};
		vector<GenericHandle> observed_components;
	};
}