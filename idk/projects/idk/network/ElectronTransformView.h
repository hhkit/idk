#pragma once
#include <core/Component.h>
#include <network/network.h>

namespace idk
{
	class ElectronTransformView
		: public Component<ElectronTransformView>
	{
	public:
		bool sync_position { true  };
		bool sync_rotation { false };
		bool sync_scale    { false };

		NetworkID GetNetworkID() const;
	};
}