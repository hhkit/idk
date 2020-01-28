#pragma once
#include <idk.h>
#include <core/Component.h>
#include <network/network.h>

namespace idk
{
	class ElectronTransformView
		: public Component<ElectronTransformView>
	{
	public:
		struct PreviousFrame
		{
			vec3 position{};
			quat rotation{};
			vec3 scale{};
			int  state_mask{};
		};

		struct GhostData
		{
			vec3 start_pos, end_pos;
			quat start_rot, end_rot;
			vec3 start_scale, end_scale;

			int state_mask{};
			real t = 1;
		};

		bool sync_position  { true  };
		bool sync_rotation  { false };
		bool sync_scale     { false };
		seconds interp_over { 1     };
		real delta_threshold{ 0.1f  };

		// interp data
		variant<void*, PreviousFrame, GhostData> ghost_data{};

		NetworkID GetNetworkID() const;
		Handle<ElectronView> GetView() const;

		// these two functions shouldn't be here
		bool UpdateMaster();
		bool UpdateGhost();
	};
}