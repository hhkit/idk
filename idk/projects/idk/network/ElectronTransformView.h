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

		struct ClientObject
		{
			struct DeltaAndValue
			{
				vec3 position; vec3 delPos;
				quat rotation; quat delRot;
				vec3 scale;    vec3 delScale;

				StateMask state_mask{};
			};

			vec3 prev_position;
			quat prev_rotation;
			vec3 prev_scale;

			sliding_window<DeltaAndValue, 64> move_window;
		};

		bool sync_position        { true  };
		bool sync_rotation        { false };
		bool sync_scale           { false };
		real interp_over_seconds  { 0.2f  };
		real send_threshold       { 0.1f  };
		real snap_threshold       { 0.5f  };

		// interp data
		variant<void*, PreviousFrame, GhostData, ClientObject> network_data{};

		NetworkID GetNetworkID() const;
		Handle<ElectronView> GetView() const;

		// these two functions shouldn't be here
		bool UpdateMaster();
		bool UpdateGhost();
	};
}