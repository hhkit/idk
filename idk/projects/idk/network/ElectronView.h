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
		vector<GenericHandle> observed_components;

		Host owner = Host::SERVER;
	};
}