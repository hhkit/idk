#include "stdafx.h"
#include "LightTypes.h"
#include <gfx/RenderTarget.h>
#include <gfx/FramebufferFactory.h>
//#include "LightTypes.h"

namespace idk
{
	constexpr ivec2 shadow_map_dim{ 512,512};

	const void* PointLight::unique_id() const noexcept
	{
		static const char test[] = "PointLight";
		return s_cast<const char*>(test);
	}
	const void* SpotLight::unique_id() const noexcept
	{
		static const char test[] = "SpotLight";
		return s_cast<const char*>(test);
	}
	const void* DirectionalLight::unique_id() const noexcept
	{
		static const char test[] = "DirectionalLight";
		return s_cast<const char*>(test);
	}
	template<typename T>
	bool NeedShadowMapImpl(T& light)
	{
		return !light.light_map;
	}
	bool NeedShadowMap(const PointLight&light){return NeedShadowMapImpl(light);};
	bool NeedShadowMap(const SpotLight&light){return NeedShadowMapImpl(light);};
	bool NeedShadowMap(const DirectionalLight&light){return NeedShadowMapImpl(light);};
	RscHandle<FrameBuffer> PointLight::InitShadowMap()
	{
		FrameBufferBuilder builder;
		builder.Begin(shadow_map_dim);
		builder.SetDepthAttachment(
			AttachmentInfo
			{
				LoadOp::eClear,
				StoreOp::eStore,
				idk::ColorFormat::DEPTH_COMPONENT,
				FilterMode::_enum::Linear,
				false //TEMP, CHANGE TO TRUE WHEN Point shadow is ready .
			}
		);
		//TODO turn it into a cube map
		auto& shadow_map = light_map = Core::GetResourceManager().GetFactory<FrameBufferFactory>().Create(builder.End());//Core::GetResourceManager().Create<FrameBuffer>();
		return shadow_map;
	}
	RscHandle<FrameBuffer> DirectionalLight::InitShadowMap()
	{
		FrameBufferBuilder builder;
		builder.Begin(shadow_map_dim);
		builder.SetDepthAttachment(
			AttachmentInfo
			{
				LoadOp::eClear,
				StoreOp::eStore,
				idk::ColorFormat::DEPTH_COMPONENT,
				FilterMode::_enum::Linear
			}
		);
		auto& shadow_map = light_map = Core::GetResourceManager().GetFactory<FrameBufferFactory>().Create(builder.End());//Core::GetResourceManager().Create<FrameBuffer>();
		return shadow_map;
	}
	RscHandle<FrameBuffer> SpotLight::InitShadowMap()
	{
		FrameBufferBuilder builder;
		builder.Begin(shadow_map_dim);
		builder.SetDepthAttachment(
			AttachmentInfo
			{
				LoadOp::eClear,
				StoreOp::eStore,
				idk::ColorFormat::DEPTH_COMPONENT,
				FilterMode::_enum::Linear
			}
		);
		auto& shadow_map = light_map = Core::GetResourceManager().GetFactory<FrameBufferFactory>().Create(builder.End());//Core::GetResourceManager().Create<FrameBuffer>();
		return shadow_map;
	}
}

