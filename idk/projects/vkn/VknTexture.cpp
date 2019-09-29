#include "pch.h"
#include "VknTexture.h"

namespace idk {
	vkn::VknTexture::~VknTexture()
	{
		//vknData.reset();
		//mem.reset();
	}

	vkn::VknTexture::VknTexture(VknTexture&& rhs)
		:Texture{ std::move(rhs) },
		size{ std::move(rhs.size) },
		sizeOnDevice{ std::move(rhs.sizeOnDevice) },
		rawData{ std::move(rhs.rawData) },
		path{ std::move(rhs.path) },
		image{ std::move(rhs.image) },
		format{ std::move(rhs.format) },
		mem{ std::move(rhs.mem) },
		imageView{ std::move(rhs.imageView) },
		sampler{ std::move(rhs.sampler) }
	{}

	vkn::VknTexture& idk::vkn::VknTexture::operator=(VknTexture&& rhs)
	{
		// TODO: insert return statement here
		Texture::operator=(std::move(rhs));

		std::swap(size, rhs.size);
		std::swap(sizeOnDevice, rhs.sizeOnDevice);
		std::swap(rawData, rhs.rawData);
		std::swap(path,rhs.path);
		std::swap(image, rhs.image);
		std::swap(format, rhs.format);
		std::swap(mem, rhs.mem);
		std::swap(imageView, rhs.imageView);
		std::swap(sampler, rhs.sampler);

		return *this;
	}

	void vkn::VknTexture::Size(ivec2 new_size)
	{
		Texture::Size(new_size);

		//ToDO update the size of texture
	}

	void* vkn::VknTexture::ID() const
	{
		//Should be descriptor set 
		return r_cast<void*>(imageView->operator VkImageView());
	}

	void vkn::VknTexture::OnMetaUpdate(const TextureMeta&)
	{

		//Update meta "does nothing now"
	}

	void vkn::VknTexture::UpdateUV(UVMode)
	{
		//pdate uv
	}


	//idk::vkn::VknTexture::VknTexture(const VknTexture& rhs)
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

