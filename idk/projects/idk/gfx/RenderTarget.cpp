#include "stdafx.h"
#include "RenderTarget.h"

namespace idk
{
	float RenderTarget::AspectRatio() const noexcept
	{
		return s_cast<float>(size.x) / size.y;
	}
	RscHandle<Texture> RenderTarget::GetColorBuffer()
	{
		return color_tex;
	}
	RscHandle<Texture> RenderTarget::GetDepthBuffer()
	{
		return depth_tex;
	}
	void RenderTarget::SetColorBuffer(RscHandle<Texture> tex)
	{
		color_tex = tex;
	}
	void RenderTarget::SetDepthBuffer(RscHandle<Texture>tex)
	{
		depth_tex = tex;
	}
	void RenderTarget::Finalize()
	{
		OnFinalize();
		_need_finalizing = false;
		int i = 0;
		if (!Name().size())
		{
			auto path = Core::GetResourceManager().GetPath(this->GetHandle());
			if(path)
				Name(*path);
		}
		for (auto& tex : this->Textures())
		{
			tex->Name(string{ Name() } + string{ names[i++] } + " Buffer");
		}
	}
	RenderTarget::~RenderTarget()
	{
		for (auto& tex : Textures())
			Core::GetResourceManager().Destroy(tex);
	}
	//Override and hide Saveable Dirty
	void RenderTarget::Dirty() 
	{
		Saveable_t::Dirty();
		_need_finalizing = true;
	}
	RenderTarget::ArrayRef<const RscHandle<Texture>, 2> RenderTarget::Textures() const
	{
		ArrayRef<const RscHandle<Texture>, 2> res;
		
		res.Set(kColorIndex, color_tex);
		res.Set(kDepthIndex, depth_tex);

		return res;
	}
	RenderTarget::ArrayRef<RscHandle<Texture>, 2>  RenderTarget::Textures()
	{
		RenderTarget::ArrayRef<RscHandle<Texture>, 2>  res;

		res.Set(kColorIndex, color_tex);
		res.Set(kDepthIndex, depth_tex);
		return res;
	}

}
