#include "stdafx.h"
#include "RenderTarget.h"
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <res/ResourceHandle.inl>

#include <gfx/ColorGrade.h>

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
		if (this->ColorGradingLut == RscHandle<Texture>{})
		{
			this->ColorGradingLut = RscHandle<Texture>{GetDefaultColorGradeGuid()};
		}
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
			tex->Name(' '+string{ Name() } +':'+ string{ names[i++] } +" Buffer");
		}
	}
	RenderTarget::~RenderTarget()
	{
		for (auto& tex : Textures())
			Core::GetResourceManager().Free(tex);
	}
	void  RenderTarget::RenderDebug(bool new_val) noexcept 
	{
		render_debug = new_val; 
	}
	void  RenderTarget::IsWorldRenderer(bool new_val) noexcept { is_world_renderer = new_val; Dirty(); }
	void  RenderTarget::Srgb(bool  srgb) 
	{
		if (is_srgb != srgb) 
		{
			is_srgb = srgb; 
			Dirty(); 
		} 
	}
	void RenderTarget::Size(uvec2 new_size)
	{
		if (size != new_size)
		{
			size = new_size;
			Dirty();
		}
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
