#include "pch.h"
#include "VknTexture.h"
#include <vkn/DDSLoader.h>
#include <vkn/VulkanTextureFactory.h>
#include <res/ResourceManager.inl>
namespace idk::vkn {
	VknTexture::VknTexture(const CompiledTexture& compiled_tex)
	{
		string_view data = { reinterpret_cast<const char*>(compiled_tex.pixel_buffer.data()), compiled_tex.pixel_buffer.size()};
		Core::GetResourceManager().GetFactory<VulkanTextureFactory>().GetDdsLoader().LoadTexture(*this,data,compiled_tex);
	}
	vk::ImageAspectFlags VknTexture::ImageAspects()
	{
		return img_aspect;
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
		// TODO: insert return statement here
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

	ivec2 VknTexture::Size(ivec2 new_size)
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

