#pragma once
#include <vkn/RenderBindings.inl>

namespace idk::vkn::bindings
{
	class SkyboxBindings : public RenderBindings
	{
	public:

		void SetCamera(const CameraData& cam_data)
		{
			_camera = cam_data;
		}


		//bool Skip(RenderInterface&, const  RenderObject&) override { return false; }
		//Stuff that should be bound at the start, before the renderobject/animated renderobject loop.
		void Bind(RenderInterface& the_interface) override;
		//Stuff that needs to be bound with every renderobject/animated renderobject
		void Bind(RenderInterface& the_interface, const  RenderObject& dc) override;

	private:
		CameraData _camera;
	};
}