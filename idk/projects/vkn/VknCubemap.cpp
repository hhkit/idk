#include "pch.h"
#include "VknCubemap.h"
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
namespace idk::vkn {
	RscHandle<Texture> VknCubemap::Tex() const noexcept
	{
		return RscHandle<Texture>{texture};
	}
	VknCubemap::~VknCubemap()
	{
		Core::GetResourceManager().Release(texture);
	}

	VknCubemap::VknCubemap(VknCubemap&& rhs) noexcept
		:CubeMap{ std::move(rhs) },
		texture{rhs.texture}
		//size{ std::move(rhs.size) },
		//sizeOnDevice{ std::move(rhs.sizeOnDevice) },
		//rawData{ std::move(rhs.rawData) },
		//path{ std::move(rhs.path) },
		//image{ std::move(rhs.image) },
		//format{ std::move(rhs.format) },
		//mem{ std::move(rhs.mem) },
		//imageView{ std::move(rhs.imageView) },
		//sampler{ std::move(rhs.sampler) }
	{
		rhs.texture = RscHandle<VknTexture>{};
	}

	VknCubemap& VknCubemap::operator=(VknCubemap&& rhs) noexcept
	{
		// TODO: insert return statement here
		CubeMap::operator=(std::move(rhs));
		std::swap(texture, rhs.texture);
		//std::swap(size, rhs.size);
		//std::swap(sizeOnDevice, rhs.sizeOnDevice);
		//std::swap(rawData, rhs.rawData);
		//std::swap(path, rhs.path);
		//std::swap(image, rhs.image);
		//std::swap(format, rhs.format);
		//std::swap(mem, rhs.mem);
		//std::swap(imageView, rhs.imageView);
		//std::swap(sampler, rhs.sampler);

		return *this;
	}

	void VknCubemap::Size(ivec2 new_size)
	{
		CubeMap::Size(new_size);
	}

	void* VknCubemap::ID() const
	{
		return texture->ID();
	}

	void VknCubemap::SetConvoluted(const RscHandle<VknCubemap>& new_cubemap)
	{
		_convoluted = new_cubemap;
	}

	RscHandle<VknCubemap> VknCubemap::GetConvoluted() const
	{
		return _convoluted;
	}

	void VknCubemap::OnMetaUpdate(const CubeMapMeta&)
	{

		//Update meta "does nothing now"
	}

	void VknCubemap::UpdateUV(UVMode)
	{
		//pdate uv
	}
};