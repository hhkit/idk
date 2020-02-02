#pragma once
#include <idk.h>
#include <core/Component.h>
#include <ds/sliding_window.inl>
#include <network/network.h>

namespace idk
{
	class ElectronTransformView
		: public Component<ElectronTransformView>
	{
	public:
		bool sync_position        { true  };
		bool sync_rotation        { false };
		bool sync_scale           { false };
		real interp_over_seconds  { 0.2f  };
		real send_threshold       { 0.1f  };
		real snap_threshold       { 0.5f  };

		NetworkID GetNetworkID() const;
		Handle<ElectronView> GetView() const;

		void Start();
	};
}