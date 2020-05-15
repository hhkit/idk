#include "pch.h"
#include "VknTexture.h"
#include <vkn/DDSLoader.h>
#include <vkn/VulkanTextureFactory.h>
#include <res/ResourceManager.inl>
#include <sstream>
#include <vkn/TextureTracker.h>

#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/VknAsyncTexLoader.h>
#include <vkn/VknTextureData.h>
namespace idk::vkn {
	namespace hlp
	{
		string DumpAllocators();
		std::pair<size_t, size_t> DumpAllocator(std::ostream& out, const MemoryAllocator& alloc);
	}
	void DoNothing();
	static size_t counter = 0;
	static size_t texture_bytes=0;
	VknTexture::VknTexture(const CompiledTexture& compiled_tex)
	{
		if (compiled_tex.pixel_buffer.size() == 0)
			return;
		string_view data = { reinterpret_cast<const char*>(compiled_tex.pixel_buffer.data()), compiled_tex.pixel_buffer.size()};
		++counter;
		auto& loader = Core::GetResourceManager().GetFactory<VulkanTextureFactory>().GetDdsLoader();
		//if (counter == 103)
		//{
		//	string test = hlp::DumpAllocators();
		//	std::stringstream ss;
		//	hlp::DumpAllocator(ss, loader.Allocator());
		//	string test2 = ss.str();
		//	DoNothing();
		//}
		//loader.LoadTexture(*this,data, compiled_tex);
		auto info = loader.GenerateTexInfo(data, compiled_tex);
		_tex_load_info = info;
		if (compiled_tex.wait_loaded)
		{
			loader.LoadTexture(*this,*_tex_load_info);
		}
		else
		{
			MarkLoaded(false);
			BeginAsyncReload(compiled_tex.guid);
		}
		texture_bytes += this->sizeOnDevice;
	}
	vk::ImageAspectFlags VknTexture::ImageAspects() const
	{
		return GetEffective().img_aspect;
	}
	uint32_t& VknTexture::Layers(uint32_t layers) noexcept
	{
		return _layers = layers;
	}
	uint32_t VknTexture::Layers()const noexcept
	{
		return GetEffective()._layers;
	}
	vk::ImageSubresourceRange VknTexture::FullRange() const
	{
		return GetEffective().range;
	}
	void VknTexture::FullRange(vk::ImageSubresourceRange range_)
	{
		range = range_;
	}
	VknTexture::~VknTexture()
	{
		//vknData.reset();
		//mem.reset();
		if(image_)
			dbg::TextureTracker::Inst().reg_deallocate(image_->operator VkImage());
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

	vk::Sampler VknTexture::Sampler() const 
	{
		return *GetEffective().sampler;
	}

	vk::Image VknTexture::Image(bool ignore_effective) const 
	{ 

		return *GetEffective(ignore_effective).image_;
	}

	vk::ImageView VknTexture::ImageView() const 
	{
		return *GetEffective().imageView;
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

	bool VknTexture::MarkLoaded(bool loaded)
	{
		return _loaded = loaded;
	}

	bool VknTexture::IsLoaded() const
	{
		return _loaded;
	}

	void VknTexture::SetTexLoadInfo(std::optional<AsyncTexLoadInfo> info)
	{
		_tex_load_info = std::move(info);
	}

	const std::optional<AsyncTexLoadInfo>& VknTexture::GetTexLoadInfo() const
	{
		return _tex_load_info;
	}

	void VknTexture::BeginAsyncReload(std::optional<Guid> guid)
	{
		if (_tex_load_info)
		{
			auto copy = *_tex_load_info;
			Core::GetSystem<VulkanWin32GraphicsSystem>().GetAsyncTexLoader().Load(std::move(copy), *this, guid ? RscHandle<VknTexture>{ *guid } : RscHandle < VknTexture>{ GetHandle() });
		}
	}

	const VknTexture& VknTexture::GetEffective(bool ignore_default) const
	{
		return (ignore_default||IsLoaded()) ? *this : *RscHandle<VknTexture>{};
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

