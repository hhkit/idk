#include "pch.h"
#include "VknTextureLoader.h"
#include <vkn/MemoryAllocator.h>
#include <vkn/BufferHelpers.h>
#include <vkn/VknTexture.h>
#include <vkn/VulkanView.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/utils/utils.h> //ReverseMap
namespace idk::vkn
{
	struct TextureResult
	{
		vk::UniqueImage first;
		hlp::UniqueAlloc second;
		vk::ImageAspectFlags aspect;
	};
	vk::UniqueImageView CreateImageView2D(vk::Device device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspect);
	TextureResult LoadTexture(hlp::MemoryAllocator& allocator, vk::Fence fence, const void* data, uint32_t width, uint32_t height, size_t len, vk::Format format, bool isRenderTarget = false);

	vk::Format    MapFormat(TextureFormat tf);
	TextureFormat MapFormat(vk::Format    tf);
	
	void TextureLoader::LoadTexture(VknTexture& texture, TextureFormat pixel_format, std::optional<TextureOptions> options,
		string_view rgba32, ivec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence, bool isRenderTarget)
	{
		LoadTexture(texture, pixel_format,options, rgba32.data(), rgba32.size(), size, allocator, load_fence, isRenderTarget);
	}

	enum class UvAxis
	{
		eU,
		eV,
		eW,
	};

	vk::SamplerAddressMode GetRepeatMode(TextureOptions options, [[maybe_unused]]UvAxis axis)
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
		static const hash_table<FilterMode, vk::Filter> map
		{
			{FilterMode::eNearest,vk::Filter::eNearest},
			{FilterMode::eLinear ,vk::Filter::eLinear},
			{FilterMode::eCubic  ,vk::Filter::eCubicIMG},
		};
		auto itr = map.find(filter_mode);
		if (itr != map.end())
			mode = itr->second;

		return mode;
	}



	vk::CompareOp MapCompareOp(CompareOp compare_op)
	{
		vk::CompareOp mode = vk::CompareOp::eNever;
		static const hash_table<CompareOp, vk::CompareOp> map
		{
			{CompareOp::eNever,			  vk::CompareOp::eNever          },
			{CompareOp::eLess,			  vk::CompareOp::eLess           },
			{CompareOp::eEqual ,		  vk::CompareOp::eEqual          },
			{CompareOp::eLessOrEqual ,	  vk::CompareOp::eLessOrEqual    },
			{CompareOp::eGreater ,		  vk::CompareOp::eGreater        },
			{CompareOp::eNotEqual,		  vk::CompareOp::eNotEqual       },
			{CompareOp::eGreaterOrEqual , vk::CompareOp::eGreaterOrEqual },
			{CompareOp::eAlways,          vk::CompareOp::eAlways         },
		};
		auto itr = map.find(compare_op);
		if (itr != map.end())
			mode = itr->second;

		return mode;
	}

	
	void TextureLoader::LoadTexture(VknTexture& texture, TextureFormat pixel_format, std::optional<TextureOptions> ooptions, const char* rgba32, size_t len, ivec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence, bool isRenderTarget)
	{
		TextureOptions options{};
		if (ooptions)
			options = *ooptions;
		auto& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		//2x2 image Checkered

		const void* rgba = rgba32;
		auto format = MapFormat(pixel_format);
		auto ptr = &texture;
		auto&& [image, alloc,aspect] = vkn::LoadTexture(allocator, load_fence, rgba, size.x, size.y, len, format, isRenderTarget);
		ptr->img_aspect = aspect;
		ptr->image = std::move(image);
		ptr->mem_alloc = std::move(alloc);
		//TODO set up Samplers and Image Views

		auto device = *view.Device();
		ptr->imageView = CreateImageView2D(device, *ptr->image, format,ptr->img_aspect);

		vk::SamplerCreateInfo sampler_info
		{
			vk::SamplerCreateFlags{},
			GetFilterMode(options,FilterType::eMin),
			GetFilterMode(options,FilterType::eMag),
			vk::SamplerMipmapMode::eNearest,
			GetRepeatMode(options,UvAxis::eU),
			GetRepeatMode(options,UvAxis::eV),
			GetRepeatMode(options,UvAxis::eW),
			0.0f,
			VK_TRUE,
			options.anisoptrophy,
			s_cast<bool>(options.compare_op),//Used for percentage close filtering
			(options.compare_op)?MapCompareOp(*options.compare_op): vk::CompareOp::eNever,
			0.0f,0.0f,
			vk::BorderColor::eFloatOpaqueBlack,
			VK_FALSE

		};
		ptr->sampler = device.createSamplerUnique(sampler_info);

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




	vk::UniqueImageView CreateImageView2D(vk::Device device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspect)
	{
		vk::ImageViewCreateInfo viewInfo{
			vk::ImageViewCreateFlags{},
			image						   ,//image                           
			vk::ImageViewType::e2D		   ,//viewType                        
			format	   ,//format                          
			vk::ComponentMapping{},
			vk::ImageSubresourceRange
		{
			aspect                         ,//aspectMask     
			0							   ,//baseMipLevel   
			1							   ,//levelCount     
			0							   ,//baseArrayLayer 
			1							   	//layerCount     
		}
		};
		return device.createImageViewUnique(viewInfo);

	}

	TextureResult LoadTexture(hlp::MemoryAllocator& allocator, vk::Fence fence, const void* data, uint32_t width, uint32_t height, size_t len, vk::Format format, bool is_render_target)
	{
		TextureResult result;
		VulkanView& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		vk::PhysicalDevice pd = view.PDevice();
		vk::Device device = *view.Device();
		auto ucmd_buffer = hlp::BeginSingleTimeCBufferCmd(device, *view.Commandpool());
		auto cmd_buffer = *ucmd_buffer;
		//bool is_render_target = isRenderTarget;

		size_t num_bytes = len;

		vk::ImageUsageFlags attachment_type = (format==vk::Format::eD16Unorm)? vk::ImageUsageFlagBits::eDepthStencilAttachment:vk::ImageUsageFlagBits::eColorAttachment;
		vk::ImageLayout     attachment_layout = vk::ImageLayout::eGeneral;//(format == vk::Format::eD16Unorm) ? vk::ImageLayout::eDepthStencilAttachmentOptimal :vk::ImageLayout::eColorAttachmentOptimal;
		std::optional<vk::ImageSubresourceRange> range{};

		vk::ImageCreateInfo imageInfo{};
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.extent.width = static_cast<uint32_t>(width);
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = 1; //1 texel deep, can't put 0, otherwise it'll be an array of 0 2D textures
		imageInfo.mipLevels = 1; //Currently no mipmapping
		imageInfo.arrayLayers = 1;
		imageInfo.format = format; //Unsigned normalized so that it can still be interpreted as a float later
		imageInfo.tiling = vk::ImageTiling::eOptimal; //We don't intend on reading from it afterwards
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = vk::ImageUsageFlagBits::eSampled | ((is_render_target) ? attachment_type : vk::ImageUsageFlagBits::eTransferDst)| vk::ImageUsageFlagBits::eTransferDst| vk::ImageUsageFlagBits::eTransferSrc; //Image needs to be transfered to and Sampled from
		imageInfo.sharingMode = vk::SharingMode::eExclusive; //Only graphics queue needs this.
		imageInfo.samples = vk::SampleCountFlagBits::e1; //Multisampling

		vk::UniqueImage image = device.createImageUnique(imageInfo, nullptr, vk::DispatchLoaderDefault{});
		auto alloc = allocator.Allocate(*image, vk::MemoryPropertyFlagBits::eDeviceLocal);
		device.bindImageMemory(*image, alloc->Memory(), alloc->Offset(), vk::DispatchLoaderDefault{});

		auto&& [stagingBuffer, stagingMemory] = hlp::CreateAllocBindBuffer(pd, device, num_bytes, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, vk::DispatchLoaderDefault{});

		if(data)
			hlp::MapMemory(device, *stagingMemory, 0, data, num_bytes, vk::DispatchLoaderDefault{});
		vk::AccessFlags src_flags = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eShaderRead;
		vk::AccessFlags dst_flags = vk::AccessFlagBits::eTransferWrite;
		vk::PipelineStageFlags shader_flags = vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader;// | vk::PipelineStageFlagBits::eTessellationControlShader | vk::PipelineStageFlagBits::eTessellationEvaluationShader;
		vk::PipelineStageFlags src_stages = shader_flags;
		vk::PipelineStageFlags dst_stages = vk::PipelineStageFlagBits::eTransfer;
		vk::ImageLayout layout = vk::ImageLayout::eGeneral;
		vk::ImageLayout next_layout = vk::ImageLayout::eTransferDstOptimal;
		if (is_render_target)
		{
			src_flags |= vk::AccessFlagBits::eColorAttachmentRead;
			src_stages |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dst_flags |= vk::AccessFlagBits::eColorAttachmentWrite;
			dst_stages |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
			next_layout = layout = attachment_layout;
		}
		vk::ImageAspectFlagBits img_aspect = vk::ImageAspectFlagBits::eColor;
		if ((format == vk::Format::eD16Unorm))
			img_aspect = vk::ImageAspectFlagBits::eDepth;
		result.aspect = img_aspect;
		TransitionImageLayout(cmd_buffer, src_flags, src_stages, dst_flags, dst_stages, vk::ImageLayout::eUndefined, next_layout, *image, img_aspect);
		if (!is_render_target)
		{

			//Copy data from buffer to image
			vk::BufferImageCopy region{};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;

			region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;

			region.imageOffset = { 0, 0, 0 };
			region.imageExtent = {
				width,
				height,
				1
			};
			cmd_buffer.copyBufferToImage(*stagingBuffer, *image, vk::ImageLayout::eTransferDstOptimal, region, vk::DispatchLoaderDefault{});

			TransitionImageLayout(cmd_buffer, src_flags, shader_flags, dst_flags, dst_stages, vk::ImageLayout::eTransferDstOptimal, layout, *image);
			;
		}


		device.resetFences(fence);
		hlp::EndSingleTimeCbufferCmd(cmd_buffer, view.GraphicsQueue(), false, fence);
		uint64_t wait_for_milli_seconds = 1;
		[[maybe_unused]] uint64_t wait_for_micro_seconds = wait_for_milli_seconds * 1000;
		//uint64_t wait_for_nano_seconds = wait_for_micro_seconds * 1000;
		while(device.waitForFences(fence, VK_TRUE,wait_for_milli_seconds)==vk::Result::eTimeout);
		result.first = std::move(image);
		result.second = std::move(alloc);
		return std::move(result);//std::pair<vk::UniqueImage, hlp::UniqueAlloc>{, };

	}

	hash_table<TextureFormat, vk::Format> FormatMap()
	{
		return hash_table<TextureFormat, vk::Format>
		{
			{TextureFormat::eD16Unorm, vk::Format::eD16Unorm},
			{TextureFormat::eRGBA32, vk::Format::eR8G8B8A8Unorm},
			{ TextureFormat::eBGRA32, vk::Format::eB8G8R8A8Unorm },
			{ TextureFormat::eBC1,vk::Format::eBc1RgbaUnormBlock },
			{ TextureFormat::eBC2,vk::Format::eBc2UnormBlock },
			{ TextureFormat::eBC3,vk::Format::eBc3UnormBlock },
			{ TextureFormat::eBC4,vk::Format::eBc4UnormBlock },
			{ TextureFormat::eBC5,vk::Format::eBc5UnormBlock },

		};

	}

	vk::Format MapFormat(TextureFormat tf)
	{
		static const auto map = FormatMap();
		return map.find(tf)->second;
	}
	TextureFormat MapFormat(vk::Format tf)
	{
		static const auto map = hlp::ReverseMap(FormatMap());
		return map.find(tf)->second;
	}
}