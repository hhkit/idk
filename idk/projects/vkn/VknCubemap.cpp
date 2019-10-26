#include "pch.h"
#include "VknCubemap.h"
namespace idk::vkn {
	VknCubemap::~VknCubemap()
	{
	}

	VknCubemap::VknCubemap(VknCubemap&& rhs) noexcept
		:CubeMap{ std::move(rhs) },
		size{ std::move(rhs.size) },
		sizeOnDevice{ std::move(rhs.sizeOnDevice) },
		rawData{ std::move(rhs.rawData) },
		path{ std::move(rhs.path) },
		image{ std::move(rhs.image) },
		format{ std::move(rhs.format) },
		mem{ std::move(rhs.mem) },
		imageView{ std::move(rhs.imageView) },
		sampler{ std::move(rhs.sampler) }
	{
	}

	VknCubemap& VknCubemap::operator=(VknCubemap&& rhs) noexcept
	{
		// TODO: insert return statement here
		CubeMap::operator=(std::move(rhs));

		std::swap(size, rhs.size);
		std::swap(sizeOnDevice, rhs.sizeOnDevice);
		std::swap(rawData, rhs.rawData);
		std::swap(path, rhs.path);
		std::swap(image, rhs.image);
		std::swap(format, rhs.format);
		std::swap(mem, rhs.mem);
		std::swap(imageView, rhs.imageView);
		std::swap(sampler, rhs.sampler);

		return *this;
	}

	void VknCubemap::Size(ivec2 new_size)
	{
		CubeMap::Size(new_size);
	}

	void* VknCubemap::ID() const
	{
		return r_cast<void*>(imageView->operator VkImageView());
	}

	void VknCubemap::SetConvoluted(const RscHandle<VknCubemap>& new_cubemap)
	{
		_convoluted = new_cubemap;
	}

	void VknCubemap::OnMetaUpdate(const CubeMapMeta&)
	{

		//Update meta "does nothing now"
	}

	void VknCubemap::UpdateUV(CMUVMode)
	{
		//pdate uv
	}
};