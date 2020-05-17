#include "pch.h"
#include "VknTextureLoader.h"
#include <vkn/MemoryAllocator.h>
#include <vkn/BufferHelpers.h>
#include <vkn/VknTexture.h>
#include <vkn/VknTextureData.h>
#include <vkn/VulkanView.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/utils/utils.h> //ReverseMap
#include <iostream>

#include <vkn/DebugUtil.h>
#include <vkn/TextureTracker.h>

#include <parallel/ThreadPool.h>

namespace std
{

	template<>
	struct hash<idk::FilterMode>
	{
		size_t operator()(idk::FilterMode mode)const { return static_cast<size_t>(mode); }
	};
}

void dbg_chk(vk::Image img);
namespace idk::vkn
{

	size_t ComputeTextureLength(size_t width, size_t height, vk::Format format);
	struct TextureResult
	{
		vk::UniqueImage first;
		hlp::UniqueAlloc second;
		vk::ImageAspectFlagBits aspect;
		size_t size_on_device = 0;
		string name;
		std::optional< std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory>> staging_data;
	};
	TextureResult LoadTexture(TextureLoader::SubmissionObjs sub, hlp::MemoryAllocator& allocator, const TexCreateInfo& load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid);
	TextureResult LoadTexture(hlp::MemoryAllocator& allocator, vk::Fence fence, const TexCreateInfo& load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid);



	enum class UvAxis
	{
		eU,
		eV,
		eW,
	};

	vk::SamplerAddressMode GetRepeatMode(TextureOptions options, [[maybe_unused]] UvAxis axis)
	{
		auto repeat_mode = options.uv_mode;
		vk::SamplerAddressMode mode = vk::SamplerAddressMode::eClampToEdge;
		static const hash_table<UVMode::_enum, vk::SamplerAddressMode> map
		{
			{UVMode::_enum::Clamp,vk::SamplerAddressMode::eClampToBorder},
			{UVMode::_enum::Repeat,vk::SamplerAddressMode::eRepeat},
			{UVMode::_enum::MirrorRepeat,vk::SamplerAddressMode::eMirroredRepeat},
		};
		auto itr = map.find(repeat_mode);
		if (itr != map.end())
			mode = itr->second;
		return mode;
	}

	enum class FilterType
	{
		eMin,
		eMag
	};

	vk::Filter GetFilterMode(TextureOptions options, FilterType type)
	{
		FilterMode filter_mode = (type == FilterType::eMin) ? options.min_filter : options.mag_filter;
		vk::Filter mode = vk::Filter::eLinear;
		static const hash_table<FilterMode::_enum, vk::Filter> map
		{
			{FilterMode::_enum::Nearest,vk::Filter::eNearest},
			{FilterMode::_enum::Linear ,vk::Filter::eLinear},
			{FilterMode::_enum::Cubic  ,vk::Filter::eCubicIMG},
		};
		auto itr = map.find(filter_mode);
		if (itr != map.end())
			mode = itr->second;

		return mode;
	}


	std::optional< std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory>> TextureLoader::LoadTexture(SubmissionObjs sub, VknTextureData& texture, hlp::MemoryAllocator& allocator, std::optional<TextureOptions> ooptions, const TexCreateInfo& _load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid)
	{
		auto load_fence = sub.load_fence;
		TextureOptions options{};
		auto load_info = _load_info;
		auto format = load_info.internal_format;
		//auto nformat = NearestBlittableFormat(format, BlitCompatUsageMasks::eDst);
		//if (nformat)
		//	format = *nformat;

		if (ooptions)
		{
			options = *ooptions;
			//	format = MapFormat(options.internal_format);
		}
		auto& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		//2x2 image Checkered
		load_info.layers = std::max(load_info.layers, 1ui32);


		auto ptr = &texture;
		if (load_info.mipmap_level > 1&& Core::GetSystem<GraphicsSystem>().extra_vars.GetOptional<bool>("HalfMip",true))
		{
			load_info.mipmap_level--;
			auto offset = ComputeTextureLength(load_info.width, load_info.height, in_info->format);
			in_info->data = reinterpret_cast<const char*>(in_info->data) + offset;
			in_info->len -= offset;
			load_info.width /= 2;
			load_info.height /= 2;
		}

		auto&& [image, alloc, aspect, sz,name,staging] = vkn::LoadTexture(sub,allocator,  load_info, in_info, guid);
		ptr->Size(uvec2{ load_info.width,load_info.height });
		ptr->mipmap_level = load_info.mipmap_level;
		ptr->format = load_info.internal_format;
		ptr->img_aspect = aspect;
		ptr->usage = load_info.image_usage;
		ptr->Layers(load_info.layers);
		ptr->image_ = std::move(image);
		ptr->mem_alloc = std::move(alloc);
		ptr->sizeOnDevice = sz;
		ptr->dbg_name = std::move(name);
		ptr->range = vk::ImageSubresourceRange
		{
			aspect,
			0,
			load_info.mipmap_level,
			0,
			load_info.layers,
		};
		//TODO set up Samplers and Image Views

		auto device = *view.Device();
		ptr->imageView = CreateImageView2D(device, ptr->Image(true), format, ptr->img_aspect,
			ImageViewInfo
			{
				0,
				load_info.mipmap_level,
				0,
				load_info.layers,
				load_info.view_type,
				load_info.component_mapping
			});

		vk::SamplerCreateInfo sampler_info
		{
			vk::SamplerCreateFlags{},
			GetFilterMode(options,FilterType::eMin),
			GetFilterMode(options,FilterType::eMag),
			//vk::SamplerMipmapMode::eNearest,//
			(options.filter_mode == FilterMode::Nearest) ? vk::SamplerMipmapMode::eNearest : vk::SamplerMipmapMode::eLinear,
			GetRepeatMode(options,UvAxis::eU),
			GetRepeatMode(options,UvAxis::eV),
			GetRepeatMode(options,UvAxis::eW),
			0.0f,
			VK_TRUE,
			options.anisoptrophy,
			s_cast<bool>(options.compare_op),//Used for percentage close filtering
			(options.compare_op) ? MapCompareOp(*options.compare_op) : vk::CompareOp::eNever,
			0.0f,0.0f + load_info.mipmap_level,
			vk::BorderColor::eFloatOpaqueBlack,
			VK_FALSE

		};
		ptr->sampler = device.createSamplerUnique(sampler_info);
		return std::move(staging);
	}
	void TextureLoader::LoadTexture(VknTexture& texture, hlp::MemoryAllocator& allocator, vk::Fence load_fence, std::optional<TextureOptions> ooptional, const TexCreateInfo& load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid)
	{
		VknTextureData tmp;
		LoadTexture(tmp, allocator,load_fence, ooptional, load_info, in_info, guid);
		tmp.ApplyOnTexture(texture);
	}
	void TextureLoader::LoadTexture(VknTextureData& texture, hlp::MemoryAllocator& allocator, vk::Fence load_fence, std::optional<TextureOptions> ooptional, const TexCreateInfo& load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid)
	{
		LoadTexture(SubmissionObjs{ {},load_fence }, texture, allocator, ooptional, load_info, in_info, guid);
	}
	namespace Nope
	{
		struct Derp
		{
			std::variant<VknTextureData*,VknTexture*> texture;
			hlp::MemoryAllocator& allocator;
			FenceObj load_fence;
			CmdBufferObj cmd_buffer;
			std::optional<TextureOptions> ooptional;
			TexCreateInfo load_info;
			std::optional<InputTexInfo> in_info;
			std::optional<Guid> guid;
			int usage = 0;
			FencePool* fp;
		};
	}

	static void DoNothing() {}
	mt::ThreadPool::Future<void> TextureLoader::LoadTextureAsync(VknTexture& texture, hlp::MemoryAllocator& allocator, FencePool& load_fence, CmdBufferPool& cmd_buffers, std::optional<TextureOptions> ooptional, TexCreateInfo load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid)
	{
		return LoadTextureAsync(&texture, allocator, load_fence, cmd_buffers, std::move(ooptional), load_info, std::move(in_info), guid);
	}
	mt::ThreadPool::Future<void> TextureLoader::LoadTextureAsync(VknTextureData& texture, hlp::MemoryAllocator& allocator, FencePool& load_fence, CmdBufferPool& cmd_buffers, std::optional<TextureOptions> ooptional, TexCreateInfo load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid)
	{
		return LoadTextureAsync(&texture, allocator, load_fence, cmd_buffers, std::move(ooptional), load_info, std::move(in_info), guid);
	}			
	static std::mutex fence_mutex;
	static std::unordered_set<VkFence> fences;
	mt::ThreadPool::Future<void> TextureLoader::LoadTextureAsync(std::variant<VknTextureData*, VknTexture*> texture, hlp::MemoryAllocator& allocator, FencePool& load_fence, CmdBufferPool& cmd_buffers, std::optional<TextureOptions> ooptional, TexCreateInfo load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid)
	{
		lock.Lock();
		auto fence = load_fence.AcquireFence();
		auto cmd_buffer = cmd_buffers.AcquireCmdBuffer();
		lock.Unlock();
		abc++;
		if (!*cmd_buffer)
			__debugbreak();
		return Core::GetThreadPool().Post(
			[loader = this,fence_pool = &load_fence](const shared_ptr<Nope::Derp>& _derp) ->void
			{
				try
				{

				auto& derp = *_derp;
				auto device = *View().Device();
				auto fence = *derp.load_fence;
				constexpr bool test_acquire_bug = false;
				if constexpr(test_acquire_bug)
				{
					std::lock_guard guard{ fence_mutex };
					if (!fences.emplace(fence.operator VkFence()).second)
						DebugBreak();
				}
				derp.usage++;
				auto& lock = loader->lock;
				VknTextureData tmp;
				bool is_data = derp.texture.index() == index_in_variant_v<VknTextureData*,decltype(derp.texture)>;
				auto& tex = (is_data) ? *std::get<VknTextureData*>(derp.texture) : tmp;
				lock.Lock();
				auto cmd_buffer = (*derp.cmd_buffer);
				cmd_buffer.begin(vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
				lock.Unlock();
				device.resetFences(fence);
				auto&& staging = loader->LoadTexture(SubmissionObjs{ cmd_buffer, vk::Fence{},false }, tex, derp.allocator, derp.ooptional, derp.load_info, derp.in_info, derp.guid);
				tex.dbg_name = tex.Name();
				dbg::NameObject(tex.Image(true), tex.dbg_name);
				lock.Lock();
				hlp::EndSingleTimeCbufferCmd(cmd_buffer, (is_data)?View().GraphicsTexQueue():View().GraphicsQueue(), false, fence);
				lock.Unlock();
				dbg_chk(tex.Image(true));
				uint64_t wait_for_milli_seconds = 0;
				[[maybe_unused]] uint64_t wait_for_micro_seconds = wait_for_milli_seconds * 0;
				//uint64_t wait_for_nano_seconds = wait_for_micro_seconds * 1000;
				while (device.waitForFences(fence, VK_TRUE, wait_for_milli_seconds) == vk::Result::eTimeout) std::this_thread::yield();

				if constexpr (test_acquire_bug)
				{
					std::lock_guard guard{ fence_mutex };
					fences.erase(fence.operator VkFence());
				}
				if (!is_data)
					tmp.ApplyOnTexture(*std::get<VknTexture*>(derp.texture));

				}
				catch (vk::Error& err)
				{
					LOG_ERROR_TO(LogPool::GFX, "LoadTexAsync Error: %s", err.what());
				}
				catch (std::exception& e)
				{
					LOG_ERROR_TO(LogPool::GFX, "LoadTexAsync Exception thrown: %s", e.what());
				}
				catch (...)
				{
					LOG_ERROR_TO(LogPool::GFX, "LoadTexAsync unknown exception thrown.");

				}
				return (void)0;
			},
			std::make_shared<Nope::Derp>(Nope::Derp{ texture, allocator, std::move(fence), std::move(cmd_buffer), ooptional, load_info, in_info, guid ,0,&load_fence}));
	}
	hash_table<TextureFormat, vk::Format> FormatMap();

	//Refer to https://www.khronos.org/registry/vulkan/specs/1.0/html/chap6.html#synchronization-access-types for access flags
	void TransitionImageLayout(vk::CommandBuffer cmd_buffer,
		vk::AccessFlags src_flags, vk::PipelineStageFlags src_stage,
		vk::AccessFlags dst_flags, vk::PipelineStageFlags dst_stage,
		vk::ImageLayout original_layout, vk::ImageLayout target, vk::Image image, std::optional<vk::ImageAspectFlags> image_aspect = {}, std::optional<vk::ImageSubresourceRange> range = {})
	{
		if (!range)
			range =
			vk::ImageSubresourceRange
		{
			(image_aspect)? *image_aspect:vk::ImageAspectFlagBits::eColor,0,1,0,1
		};
		vk::ImageMemoryBarrier barrier
		{
			src_flags,dst_flags,original_layout,target,VK_QUEUE_FAMILY_IGNORED,VK_QUEUE_FAMILY_IGNORED,image,*range
		};
		cmd_buffer.pipelineBarrier(src_stage, dst_stage, {}, nullptr, nullptr, barrier, vk::DispatchLoaderDefault{});
	}



	vk::UniqueImageView CreateImageView2D(vk::Device device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspect, ImageViewInfo info)
	{
		vk::ImageViewCreateInfo viewInfo{
			vk::ImageViewCreateFlags{},
			image						   ,//image                           
			info.type		   ,//viewType                        
			format	   ,//format                          
			info.component_mapping,
			vk::ImageSubresourceRange
		{
			aspect                         ,//aspectMask     
			info.base_mip_level    							   ,//baseMipLevel   
			info.level_count       							   ,//levelCount     
			info.base_array_layer  							   ,//baseArrayLayer 
			info.array_layer_count 							   	//layerCount     
		}
		};
		return device.createImageViewUnique(viewInfo);

	}
	size_t ceil_div(size_t n, size_t d);
	size_t ComputeTextureLength(size_t width, size_t height, vk::Format format)
	{
		size_t result=width*height*4; //default to 4bytes per pixel (Rgba)
		size_t block_size = 4;
		size_t block_width  = ceil_div(width , block_size);
		size_t block_height = ceil_div(height, block_size);
		switch (format)
		{
		case vk::Format::eR32G32B32A32Sfloat:
		case vk::Format::eR32G32B32A32Uint:
		case vk::Format::eR32G32B32A32Sint:
			result = width * height * 16;
			break;
		case vk::Format::eR16G16B16A16Sfloat:
		case vk::Format::eR16G16B16A16Unorm:
		case vk::Format::eR16G16B16A16Snorm:
		case vk::Format::eR16G16B16A16Uscaled:
		case vk::Format::eR16G16B16A16Sscaled:
		case vk::Format::eR16G16B16A16Uint:
		case vk::Format::eR16G16B16A16Sint:
			result = width * height * 8;
			break;
			case vk::Format::eR8G8B8A8Unorm        :
			case vk::Format::eR8G8B8A8Snorm        :
			case vk::Format::eR8G8B8A8Uscaled      :
			case vk::Format::eR8G8B8A8Sscaled      :
			case vk::Format::eR8G8B8A8Uint         :
			case vk::Format::eR8G8B8A8Sint         :
			case vk::Format::eR8G8B8A8Srgb         :
			case vk::Format::eB8G8R8A8Unorm        :
			case vk::Format::eB8G8R8A8Snorm        :
			case vk::Format::eB8G8R8A8Uscaled      :
			case vk::Format::eB8G8R8A8Sscaled      :
			case vk::Format::eB8G8R8A8Uint         :
			case vk::Format::eB8G8R8A8Sint         :
			case vk::Format::eB8G8R8A8Srgb         :
			case vk::Format::eA8B8G8R8UnormPack32  :
			case vk::Format::eA8B8G8R8SnormPack32  :
			case vk::Format::eA8B8G8R8UscaledPack32:
			case vk::Format::eA8B8G8R8SscaledPack32:
			case vk::Format::eA8B8G8R8UintPack32   :
			case vk::Format::eA8B8G8R8SintPack32   :
			case vk::Format::eA8B8G8R8SrgbPack32   :
				result = width * height * 4;
				break;
			case vk::Format::eBc1RgbUnormBlock :
			case vk::Format::eBc1RgbSrgbBlock  :
			case vk::Format::eBc1RgbaUnormBlock:
			case vk::Format::eBc1RgbaSrgbBlock :
			case vk::Format::eBc4UnormBlock    :
			case vk::Format::eBc4SnormBlock    :
				result = block_width * block_height * 8;
			break;
			case vk::Format::eBc2UnormBlock:
			case vk::Format::eBc2SrgbBlock :
			case vk::Format::eBc3UnormBlock:
			case vk::Format::eBc3SrgbBlock :
            case vk::Format::eBc5UnormBlock: 
            case vk::Format::eBc5SnormBlock:
			result = block_width * block_height * 16;
			break;
		}
		return result;
	}
	TexCreateInfo ColorBufferTexInfo(uint32_t width, uint32_t height)
	{
		TexCreateInfo info{};
		info.width = width;
		info.height = height;
		info.internal_format = vk::Format::eB8G8R8A8Srgb;
		info.image_usage = vk::ImageUsageFlagBits::eColorAttachment;
		info.aspect = vk::ImageAspectFlagBits::eColor;
		info.sampled(true);
		return info;
	}

	TexCreateInfo DepthBufferTexInfo(uint32_t width, uint32_t height)
	{
		TexCreateInfo info{};
		info.width = width;
		info.height = height;
		info.internal_format = vk::Format::eD32Sfloat;
		info.image_usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
		info.aspect = vk::ImageAspectFlagBits::eDepth;
		info.sampled(true);
		return info;
	}
	TexCreateInfo StencilBufferTexInfo(uint32_t width, uint32_t height)
	{
		TexCreateInfo info{};
		info.width = width;
		info.height = height;
		info.internal_format = vk::Format::eD32SfloatS8Uint;
		info.image_usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
		info.aspect = vk::ImageAspectFlagBits::eDepth;
		info.sampled(true);
		return info;
	}
	TexCreateInfo Depth3DBufferTexInfo(uint32_t width, uint32_t height)
	{
		TexCreateInfo info{};
		info.width = width;
		info.height = height;
		info.internal_format = vk::Format::eD16Unorm;
		info.image_usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
		info.aspect = vk::ImageAspectFlagBits::eDepth;
		info.view_type = vk::ImageViewType::eCube;
		info.image_create_bits = vk::ImageCreateFlagBits::eCubeCompatible;
		info.sampled(true);
		info.layers = 6;
		return info;
	}
	std::pair<vk::UniqueImage, hlp::UniqueAlloc> CreateBlitImage(hlp::MemoryAllocator& allocator,uint32_t mipmap_level, uint32_t width, uint32_t height, vk::Format format)
	{

		VulkanView& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		vk::PhysicalDevice pd = view.PDevice();
		vk::Device device = *view.Device();

		vk::ImageCreateInfo imageInfo{};
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.extent.width = static_cast<uint32_t>(width);
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = 1; //1 texel deep, can't put 0, otherwise it'll be an array of 0 2D textures
		imageInfo.mipLevels = mipmap_level+1; //Currently no mipmapping
		imageInfo.arrayLayers = 1;
		imageInfo.format = format; //Unsigned normalized so that it can still be interpreted as a float later
		imageInfo.tiling = vk::ImageTiling::eOptimal; //We don't intend on reading from it afterwards
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;// vk::ImageUsageFlagBits::eSampled | ((is_render_target) ? attachment_type : vk::ImageUsageFlagBits::eTransferDst) | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc; //Image needs to be transfered to and Sampled from
		imageInfo.sharingMode = vk::SharingMode::eExclusive; //Only graphics queue needs this.
		imageInfo.samples = vk::SampleCountFlagBits::e1; //Multisampling

		vk::UniqueImage image = device.createImageUnique(imageInfo, nullptr, vk::DispatchLoaderDefault{});
		auto alloc = allocator.Allocate(*image, vk::MemoryPropertyFlagBits::eDeviceLocal); //Allocate on device only
		device.bindImageMemory(*image, alloc->Memory(), alloc->Offset(), vk::DispatchLoaderDefault{});
		return std::make_pair(std::move(image), std::move(alloc));
	}

	void BlitConvert(vk::CommandBuffer cmd_buffer,vk::ImageAspectFlags aspect,vk::Image src_image,vk::Image dest_image, uint32_t mip_level,uint32_t width, uint32_t height)
	{
		int z = 1;
		vector<vk::ImageBlit> blitter(mip_level+1);
		for(uint32_t i=0;i<=mip_level;++i)
		{
			blitter[i] = vk::ImageBlit
			{
				vk::ImageSubresourceLayers{aspect,i,0,1},
				std::array<vk::Offset3D,2>{vk::Offset3D{0,0,0},vk::Offset3D{s_cast<int32_t>(width>>i),s_cast<int32_t>(height>>i),z}},
				vk::ImageSubresourceLayers{aspect,i,0,1},
				std::array<vk::Offset3D,2>{vk::Offset3D{0,0,0},vk::Offset3D{s_cast<int32_t>(width>>i),s_cast<int32_t>(height>>i),z}},
			};
			
		}
		cmd_buffer.blitImage(src_image, vk::ImageLayout::eTransferSrcOptimal, dest_image, vk::ImageLayout::eTransferDstOptimal, blitter,vk::Filter::eNearest
		);
	}
	void DoNothing();
	TextureResult LoadTexture(TextureLoader::SubmissionObjs sub,hlp::MemoryAllocator& allocator,  const TexCreateInfo& load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid)
	{
		vk::CommandBuffer cmd_buffer;
		vk::UniqueCommandBuffer ucmd_buffer;
		if (!sub.cmd_buffer)
		{
			VulkanView& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
			vk::PhysicalDevice pd = view.PDevice();
			vk::Device device = *view.Device();
			ucmd_buffer = hlp::BeginSingleTimeCBufferCmd(device, *view.Commandpool());
			cmd_buffer = *ucmd_buffer;
		}
		else
		{
			cmd_buffer = *sub.cmd_buffer;;
		}
		vk::Fence fence = sub.load_fence;
		auto format = load_info.internal_format, internal_format = load_info.internal_format;
		TextureResult result;
		VulkanView& view = View();
		vk::PhysicalDevice pd = view.PDevice();
		vk::Device device = *view.Device();
		//auto format = load_info.internal_format;
		//bool is_render_target = isRenderTarget;
		size_t len;
		uint32_t width = load_info.width, height = load_info.height;

		vk::ImageUsageFlags image_usage = load_info.image_usage;//(format == vk::Format::eD16Unorm) ? vk::ImageUsageFlagBits::eDepthStencilAttachment : vk::ImageUsageFlagBits::eColorAttachment;
		//vk::ImageLayout     attachment_layout = vk::ImageLayout::eGeneral;//(format == vk::Format::eD16Unorm) ? vk::ImageLayout::eDepthStencilAttachmentOptimal :vk::ImageLayout::eColorAttachmentOptimal;

		size_t single_level_size = ComputeTextureLength(width, height, format);
		if (load_info.view_type == vk::ImageViewType::eCube)
			single_level_size *= 6;
		
		if (!in_info) { //If data isn't given.
			len = single_level_size;
		}
		else
		{
			len = in_info->len;
			format = in_info->format;
			image_usage |= vk::ImageUsageFlagBits::eTransferDst;
		}
		size_t num_bytes = len;

		std::optional<vk::ImageSubresourceRange> range{};

		if (load_info.layers == 0)
			throw;
		vk::MemoryPropertyFlags mem_property = vk::MemoryPropertyFlagBits::eDeviceLocal;
		//if (!(vk::ImageUsageFlagBits::eColorAttachment| vk::ImageUsageFlagBits::eDepthStencilAttachment) & load_info.image_usage)
		//{
		//	
		//}
		vk::ImageCreateInfo imageInfo{};
		imageInfo.flags = load_info.image_create_bits;
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.extent.width = static_cast<uint32_t>(width);
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = 1; //1 texel deep, can't put 0, otherwise it'll be an array of 0 2D textures
		imageInfo.mipLevels = load_info.mipmap_level; 
		imageInfo.arrayLayers =  load_info.layers;
		imageInfo.format = internal_format; //Unsigned normalized so that it can still be interpreted as a float later
		imageInfo.tiling = vk::ImageTiling::eOptimal; //We don't intend on reading from it afterwards
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = image_usage;//vk::ImageUsageFlagBits::eSampled | ((is_render_target) ? attachment_type : vk::ImageUsageFlagBits::eTransferDst) | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc; //Image needs to be transfered to and Sampled from
		imageInfo.sharingMode = vk::SharingMode::eExclusive; //Only graphics queue needs this.
		imageInfo.samples = vk::SampleCountFlagBits::e1; //Multisampling

		//if (imageInfo.mipLevels > 1)
		//{
		//	imageInfo.mipLevels--;
		//	width = imageInfo.extent.width /= 2;
		//	height = imageInfo.extent.height /= 2;
		//}


		vk::UniqueImage image = device.createImageUnique(imageInfo, nullptr, vk::DispatchLoaderDefault{});
		auto alloc = allocator.Allocate(*image, mem_property); //Allocate on device only
		result.size_on_device = alloc->Size();
		device.bindImageMemory(*image, alloc->Memory(), alloc->Offset(), vk::DispatchLoaderDefault{});

		const vk::ImageAspectFlagBits img_aspect = load_info.aspect;
		result.aspect = img_aspect;
		vk::UniqueImage blit_src_img;
		hlp::UniqueAlloc blit_img_alloc;
		vk::UniqueBuffer staging_buffer;
		vk::UniqueDeviceMemory staging_memory;
		
		//Debugging
		bool conditions[] = { false,false,false,false,false,false,false };
		if (image)
			dbg::TextureTracker::Inst().reg_allocate(image->operator VkImage(),num_bytes);
		if (in_info)
		{
			conditions[0] = true;
			vk::ImageSubresourceRange sub_range;
			sub_range.aspectMask = img_aspect;
			sub_range.baseMipLevel = 0;
			sub_range.levelCount = imageInfo.mipLevels;
			sub_range.baseArrayLayer = 0;
			sub_range.layerCount = load_info.layers;

			//TODO update this part so that we check the usage flags and set access flags accordingly.
			vk::AccessFlags src_flags = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eShaderRead;
			vk::AccessFlags dst_flags = vk::AccessFlagBits::eTransferWrite;
			vk::PipelineStageFlags shader_flags = vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader;// | vk::PipelineStageFlagBits::eTessellationControlShader | vk::PipelineStageFlagBits::eTessellationEvaluationShader;
			vk::PipelineStageFlags src_stages = shader_flags;
			vk::PipelineStageFlags dst_stages = vk::PipelineStageFlagBits::eTransfer;
			vk::ImageLayout layout = vk::ImageLayout::eGeneral;
			vk::ImageLayout next_layout = vk::ImageLayout::eTransferDstOptimal;

			vk::Image copy_dest = *image;
			if (internal_format != format)
			{
				auto&& [img, al] = CreateBlitImage(allocator, imageInfo.mipLevels, width, height,format);

				blit_src_img = std::move(img);
				blit_img_alloc = std::move(al);
				copy_dest = *blit_src_img;
				layout = vk::ImageLayout::eTransferSrcOptimal;
			}
			auto&& [stagingBuffer, stagingMemory] = hlp::CreateAllocBindBuffer(pd, device, num_bytes, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, vk::DispatchLoaderDefault{});

			if (in_info)
				hlp::MapMemory(device, *stagingMemory, 0, in_info->data, num_bytes, vk::DispatchLoaderDefault{});
			//if (is_render_target)
			//{
			//	src_flags |= vk::AccessFlagBits::eColorAttachmentRead;
			//	src_stages |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
			//	dst_flags |= vk::AccessFlagBits::eColorAttachmentWrite;
			//	dst_stages |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
			//	next_layout = layout = attachment_layout;
			//}
			//if ((format == vk::Format::eD16Unorm))
			//	img_aspect = vk::ImageAspectFlagBits::eDepth;
			TransitionImageLayout(cmd_buffer, src_flags, src_stages, dst_flags, dst_stages, vk::ImageLayout::eUndefined, next_layout, copy_dest, img_aspect, sub_range);
			//if (!is_render_target)
			{

				//Copy data from buffer to image
				vector< vk::BufferImageCopy> copy_regions(imageInfo.mipLevels);
				{
					vk::BufferImageCopy region{};
					region.bufferOffset = 0;
					region.bufferRowLength = 0;
					region.bufferImageHeight = 0;

					region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					region.imageSubresource.mipLevel = 0;// std::max(load_info.mipmap_level, 1u) - 1;
					region.imageSubresource.baseArrayLayer = 0;
					region.imageSubresource.layerCount = load_info.layers;

					region.imageOffset = { 0, 0, 0 };
					region.imageExtent = {
						width      ,
						height ,
						1
					};
					copy_regions[0] = (region);
				}
				size_t offset = single_level_size;
				for (uint32_t i = 1; i < imageInfo.mipLevels; ++i)
				{
				vk::BufferImageCopy region{};
				region.bufferOffset = offset;
				region.bufferRowLength = 0;
				region.bufferImageHeight = 0;

				region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
				region.imageSubresource.mipLevel = i;// std::max(load_info.mipmap_level, 1u) - 1;
				region.imageSubresource.baseArrayLayer = 0;
				region.imageSubresource.layerCount = load_info.layers;

				region.imageOffset = { 0, 0, 0 };
				region.imageExtent = {
					width >>i,
					height >>i,
					1
				};
				copy_regions[i]=(region);
				auto lenggth = ComputeTextureLength(region.imageExtent.width, region.imageExtent.height, format);
				if (load_info.view_type == vk::ImageViewType::eCube)
					lenggth *= 6;
				offset += lenggth;//single_level_size >> (2 * i);
				}
				//vector<vk::BufferImageCopy>

				//if (View().DynDispatcher().vkSetDebugUtilsObjectNameEXT)
				//{
				//	auto name = string{ *guid };
				//	if (name == "21707462-3865-4559-b88b-f816025d22a2")
				//	{
				//		DoNothing();
				//	}
				//	name += " Staging Dest";
				//	dbg::NameObject(copy_dest, name);
				//}
				cmd_buffer.copyBufferToImage(*stagingBuffer, copy_dest, vk::ImageLayout::eTransferDstOptimal, copy_regions, vk::DispatchLoaderDefault{});

				staging_buffer = std::move(stagingBuffer);
				staging_memory = std::move(stagingMemory);
				//TransitionImageLayout(cmd_buffer, src_flags, shader_flags, dst_flags, dst_stages, vk::ImageLayout::eTransferDstOptimal, layout, copy_dest);
				;
			}


			if (internal_format != format)
			{
				conditions[1] = true;
				TransitionImageLayout(cmd_buffer, vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eTransferRead, vk::PipelineStageFlagBits::eTransfer, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, copy_dest, img_aspect,sub_range);
				TransitionImageLayout(cmd_buffer, {}, vk::PipelineStageFlagBits::eAllCommands , vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTransfer, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, * image, img_aspect, sub_range);
				BlitConvert(cmd_buffer, img_aspect, *blit_src_img, *image, imageInfo.mipLevels, width, height);
			}
			TransitionImageLayout(cmd_buffer, vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTransfer, dst_flags, dst_stages, vk::ImageLayout::eTransferDstOptimal, load_info.layout, *image, img_aspect,sub_range);

	
		}else if(imageInfo.initialLayout!=load_info.layout)
		{
			conditions[2] = true;
			TransitionImageLayout(cmd_buffer, vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTransfer, {}, vk::PipelineStageFlagBits::eAllCommands, vk::ImageLayout::eUndefined, load_info.layout, * image, img_aspect);
		}
		//else
		//{
		//	TransitionImageLayout(cmd_buffer, vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTransfer, {}, vk::PipelineStageFlagBits::eAllCommands, vk::ImageLayout::eUndefined, load_info.layout, * image, img_aspect);
		//}
		if (sub.end_and_submit)
		{
			if (!fence)
				DebugBreak();
			device.resetFences(fence);
			hlp::EndSingleTimeCbufferCmd(cmd_buffer, view.GraphicsQueue(), false, fence);
			uint64_t wait_for_milli_seconds = 1;
			[[maybe_unused]] uint64_t wait_for_micro_seconds = wait_for_milli_seconds * 0;
			//uint64_t wait_for_nano_seconds = wait_for_micro_seconds * 1000;
			while (device.waitForFences(fence, VK_TRUE, wait_for_milli_seconds) == vk::Result::eTimeout);
		}
		if (!sub.cmd_buffer)
		{
			ucmd_buffer.reset();
			device.resetCommandPool(*View().Commandpool(), {});
		}
		if (View().DynDispatcher().vkSetDebugUtilsObjectNameEXT)
		{
			auto& name = result.name =  string{ *guid };
			result.name.reserve(32);
			if (name == "d7e578ab-3254-4564-bee0-1555837861f7")
			{
				DoNothing();
			}
			dbg::NameObject(*image, name);
		}

		result.first = std::move(image);
		result.second = std::move(alloc);
		if(!sub.end_and_submit)
			result.staging_data = std::pair{std::move(staging_buffer),std::move(staging_memory)};
		return std::move(result);//std::pair<vk::UniqueImage, hlp::UniqueAlloc>{, };
	}
	bool fml = false;
	bool IsDepthStencil(vk::Format format);
	void Mark() { fml = true; }
	TextureOptions::TextureOptions(const CompiledTexture& meta)
	{
		guid = meta.guid;
		min_filter = mag_filter = filter_mode = meta.filter_mode;
		uv_mode = meta.mode;
		internal_format = ToInternalFormat(meta.internal_format, input_is_srgb = meta.is_srgb);
	}
}