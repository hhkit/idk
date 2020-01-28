#include "pch.h"
#include "VknTexture.h"

namespace idk::vkn {
	vk::ImageAspectFlags VknTexture::ImageAspects()const
	{
		return img_aspect;
	}
	uint32_t& VknTexture::Layers(uint32_t layers) noexcept
	{
		return _layers = layers;
	}
	uint32_t VknTexture::Layers()const noexcept
	{
		return _layers;
	}
	VknTexture::~VknTexture()
	{
		//vknData.reset();
		//mem.reset();
	}

	VknTexture::VknTexture(VknTexture&& rhs) noexcept
		:Texture{ std::move(rhs) },
		size{ std::move(rhs.size) },
		sizeOnDevice{ std::move(rhs.sizeOnDevice) },
		rawData{ std::move(rhs.rawData) },
		path{ std::move(rhs.path) },
		image_{ std::move(rhs.image_) },
		format{ std::move(rhs.format) },
		mem{ std::move(rhs.mem) },
		imageView{ std::move(rhs.imageView) },
		sampler{ std::move(rhs.sampler) }
	{}

	VknTexture& VknTexture::operator=(VknTexture&& rhs) noexcept
	{
		Texture::operator=(std::move(rhs));

		std::swap(size, rhs.size);
		std::swap(sizeOnDevice, rhs.sizeOnDevice);
		std::swap(rawData, rhs.rawData);
		std::swap(path,rhs.path);
		std::swap(image_, rhs.image_);
		std::swap(format, rhs.format);
		std::swap(mem, rhs.mem);
		std::swap(imageView, rhs.imageView);
		std::swap(sampler, rhs.sampler);

		return *this;
	}

	uvec2 VknTexture::Size(uvec2 new_size)
	{
		return Texture::Size(new_size);

		//ToDO update the size of texture
	}

	void* VknTexture::ID() const
	{
		//Should be descriptor set 
		return r_cast<void*>(imageView->operator VkImageView());
	}

	void VknTexture::OnMetaUpdate(const TextureMeta&)
	{

		//Update meta "does nothing now"
	}

	void VknTexture::UpdateUV(UVMode)
	{
		//pdate uv
	}


	//idk::VknTexture::VknTexture(const VknTexture& rhs)
	//	:Texture{std::move(rhs)},
	//	size{rhs.size},
	//	sizeOnDevice{rhs.sizeOnDevice},
	//	rawData{ std::move(rhs.rawData)},
	//	path{ rhs.path },
	//	vknData{ std::move(*rhs.vknData) },
	//	format{ rhs.format },
	//	mem{ std::move(*rhs.mem) },
	//	imageView{std::move(*rhs.imageView)},
	//	sampler{std::move(*rhs.sampler)}
	//{}
};

