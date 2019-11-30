#pragma once
#include <idk.h>
#include "RenderObject.h"

namespace idk
{
	enum class RequestState
	{
		eInvalid,
		eNeedRender,
		eRendered
	};

	struct RenderRequest
	{
		CameraData camera;
		vector<LightData> light_data;
		vector<RenderObject> mesh_render;
		vector<AnimatedRenderObject> skinned_mesh_render;
		vector<SkeletonTransforms> skeleton_transforms;
		std::atomic<RequestState> _state = RequestState::eInvalid;

		void MarkRendered()
		{
			_state = RequestState::eRendered;
		}

		bool NeedsRendering() const
		{
			return _state == RequestState::eNeedRender;
		}

		bool Ready() const
		{
			return _state == RequestState::eRendered;
		}
		void Clear()
		{
			_state = RequestState::eInvalid;
		}
		void Set(RenderRequest&& req)
		{
			camera = std::move(req.camera);
			mesh_render = std::move(req.mesh_render);
			skinned_mesh_render = std::move(req.skinned_mesh_render);
			skeleton_transforms = std::move(req.skeleton_transforms);
			_state = RequestState::eNeedRender;
		}

	};
}