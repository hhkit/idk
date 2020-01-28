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

		struct ControlObject
		{
			int owningClient = 0;
			unsigned seq_no = 0;
			vec3 moved_position;
			quat moved_rotation;
			vec3 moved_scale;
		};

		struct ClientObject
		{
			unsigned seq_no = 0;
			vec3 prev_position;
			quat prev_rotation;
			vec3 prev_scale;
		};

		bool sync_position        { true  };
		bool sync_rotation        { false };
		bool sync_scale           { false };
		real interp_over_seconds  { 0.2f  };
		real send_threshold       { 0.1f  };
		real snap_threshold       { 0.5f  };

		// interp data
		variant<void*, PreviousFrame, GhostData, ControlObject> ghost_data{};

		NetworkID GetNetworkID() const;
		Handle<ElectronView> GetView() const;

		// these two functions shouldn't be here
		bool UpdateMaster();
		bool UpdateGhost();
	};
}