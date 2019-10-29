#pragma once
#pragma once
#include<vulkan/vulkan.hpp>
#include "idk.h"
#include "gfx/Cubemap.h"
#include <vkn/MemoryAllocator.h>
#include <vkn/VknTexture.h>
namespace idk::vkn {

	struct VknCubemap
		: public CubeMap
	{
		RscHandle<VknTexture>   texture;


		RscHandle<Texture> Tex()const noexcept;
		vk::Sampler Sampler()const { return texture->Sampler(); }
		vk::Image Image()const { return texture->Image(); }
		vk::ImageView ImageView()const { return texture->ImageView(); }
		VknCubemap() = default;
		~VknCubemap();
		//VknTexture(const VknTexture& rhs);
		VknCubemap(VknCubemap&& rhs) noexcept;

		VknCubemap& operator=(VknCubemap&&) noexcept;
		void Size(ivec2 new_size) override;
		virtual void* ID() const;

		void SetConvoluted(const RscHandle<VknCubemap>&);
		RscHandle<VknCubemap> GetConvoluted()const;

		bool is_convoluted = false;
	private:
		RscHandle<VknCubemap> _convoluted;
		void OnMetaUpdate(const CubeMapMeta&);
		void UpdateUV(CMUVMode);

	};
};