#include "pch.h"
#include "VknTextureLoader.h"
#include <vkn/MemoryAllocator.h>
#include <vkn/BufferHelpers.h>
#include <vkn/VknTexture.h>
#include <vkn/VulkanView.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/utils/utils.h> //ReverseMap
#include <iostream>
namespace std
{

	template<>
	struct hash<idk::FilterMode>
	{
		size_t operator()(idk::FilterMode mode)const { return static_cast<size_t>(mode); }
	};
}

namespace idk::vkn
{


	struct TextureResult
	{
		vk::UniqueImage first;
		hlp::UniqueAlloc second;
		vk::ImageAspectFlags aspect;
		size_t size_on_device = 0;
	};
	vk::UniqueImageView CreateImageView2D(vk::Device device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspect);
	TextureResult LoadTexture(hlp::MemoryAllocator& allocator, vk::Fence fence, const void* data, uint32_t width, uint32_t height, size_t len, vk::Format format, bool isRenderTarget = false);
	TextureResult LoadTexture(hlp::MemoryAllocator& allocator, vk::Fence fence, const TexCreateInfo& load_info, std::optional<InputTexInfo> in_info);
	
	void TextureLoader::LoadTexture(VknTexture& texture, TextureFormat pixel_format, std::optional<TextureOptions> options,
		string_view rgba32, uvec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence, bool isRenderTarget)
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

	
	void TextureLoader::LoadTexture(VknTexture& texture, hlp::MemoryAllocator& allocator, vk::Fence load_fence,std::optional<TextureOptions> ooptions, const TexCreateInfo& load_info, std::optional<InputTexInfo> in_info)
	{
		TextureOptions options{};
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

		auto ptr = &texture;
		auto&& [image, alloc, aspect] = vkn::LoadTexture(allocator, load_fence, load_info,in_info);
		ptr->Size(uvec2{load_info.width,load_info.height});
		ptr->format = load_info.internal_format;
		ptr->img_aspect = aspect;
		ptr->usage = load_info.image_usage;
		ptr->Layers(load_info.layers);
		ptr->image_ = std::move(image);
		ptr->mem_alloc = std::move(alloc);
		ptr->sizeOnDevice = sz;
		//TODO set up Samplers and Image Views

		auto device = *view.Device();
		ptr->imageView = CreateImageView2D(device, ptr->Image(), format, ptr->img_aspect);

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
			(options.compare_op) ? MapCompareOp(*options.compare_op) : vk::CompareOp::eNever,
			0.0f,0.0f,
			vk::BorderColor::eFloatOpaqueBlack,
			VK_FALSE

		};
		ptr->sampler = device.createSamplerUnique(sampler_info);

	}
	void TextureLoader::LoadTexture(VknTexture& texture, TextureFormat pixel_format, std::optional<TextureOptions> ooptions, const char* rgba32, size_t len, uvec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence, bool isRenderTarget)
	{
		TextureOptions options{};
		if (ooptions)
			options = *ooptions;
		auto& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		//2x2 image Checkered

		const void* rgba = rgba32;
		auto format = MapFormat(pixel_format);
		auto ptr = &texture;
		auto&& [image, alloc,aspect,sz] = vkn::LoadTexture(allocator, load_fence, rgba, size.x, size.y, len, format, isRenderTarget);
		ptr->img_aspect = aspect;
		ptr->sizeOnDevice = sz;
		ptr->image_ = std::move(image);
		ptr->mem_alloc = std::move(alloc);
		//TODO set up Samplers and Image Views

		auto device = *view.Device();
		ptr->imageView = CreateImageView2D(device, *ptr->image_, format,ptr->img_aspect);

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
		texture.Size(size);
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

	TextureResult LoadTexture(hlp::MemoryAllocator& allocator, vk::Fence fence, const TexCreateInfo& load_info, std::optional<InputTexInfo> in_info)
	{
		auto format = load_info.internal_format, internal_format = load_info.internal_format;
		TextureResult result;
		VulkanView& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		vk::PhysicalDevice pd = view.PDevice();
		vk::Device device = *view.Device();
		auto ucmd_buffer = hlp::BeginSingleTimeCBufferCmd(device, *view.Commandpool());
		auto cmd_buffer = *ucmd_buffer;
		//auto format = load_info.internal_format;
		//bool is_render_target = isRenderTarget;
		size_t len;
		uint32_t width = load_info.width, height = load_info.height;

		vk::ImageUsageFlags image_usage = load_info.image_usage;//(format == vk::Format::eD16Unorm) ? vk::ImageUsageFlagBits::eDepthStencilAttachment : vk::ImageUsageFlagBits::eColorAttachment;
		//vk::ImageLayout     attachment_layout = vk::ImageLayout::eGeneral;//(format == vk::Format::eD16Unorm) ? vk::ImageLayout::eDepthStencilAttachmentOptimal :vk::ImageLayout::eColorAttachmentOptimal;

		if (!in_info) { //If data isn't given.
			len = ComputeTextureLength(width, height, format);
		}
		else
		{
			len = in_info->len;
			format = in_info->format;
			image_usage |= vk::ImageUsageFlagBits::eTransferDst;
		}
		size_t num_bytes = len;

		std::optional<vk::ImageSubresourceRange> range{};

		vk::ImageCreateInfo imageInfo{};
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.extent.width = static_cast<uint32_t>(width);
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = 1; //1 texel deep, can't put 0, otherwise it'll be an array of 0 2D textures
		imageInfo.mipLevels = load_info.mipmap_level+1; //Currently no mipmapping
		imageInfo.arrayLayers = load_info.layers;
		imageInfo.format = internal_format; //Unsigned normalized so that it can still be interpreted as a float later
		imageInfo.tiling = vk::ImageTiling::eOptimal; //We don't intend on reading from it afterwards
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = image_usage;//vk::ImageUsageFlagBits::eSampled | ((is_render_target) ? attachment_type : vk::ImageUsageFlagBits::eTransferDst) | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc; //Image needs to be transfered to and Sampled from
		imageInfo.sharingMode = vk::SharingMode::eExclusive; //Only graphics queue needs this.
		imageInfo.samples = vk::SampleCountFlagBits::e1; //Multisampling
		vk::UniqueImage image = device.createImageUnique(imageInfo, nullptr, vk::DispatchLoaderDefault{});
		auto alloc = allocator.Allocate(*image, vk::MemoryPropertyFlagBits::eDeviceLocal); //Allocate on device only
		result.size_on_device = alloc->Size();
		device.bindImageMemory(*image, alloc->Memory(), alloc->Offset(), vk::DispatchLoaderDefault{});
		
		const vk::ImageAspectFlagBits img_aspect = load_info.aspect;
		result.aspect = img_aspect;
		vk::UniqueImage blit_src_img;
		hlp::UniqueAlloc blit_img_alloc;
		vk::UniqueBuffer staging_buffer;
		vk::UniqueDeviceMemory staging_memory;
		if (in_info)
		{

			vk::ImageSubresourceRange sub_range;
			sub_range.aspectMask = img_aspect;
			sub_range.baseMipLevel = 0;
			sub_range.levelCount = load_info.mipmap_level + 1;
			sub_range.baseArrayLayer = 0;
			sub_range.layerCount = 1;

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
				auto&& [img, al] = CreateBlitImage(allocator, load_info.mipmap_level, width, height,format);

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
				vector< vk::BufferImageCopy> copy_regions(load_info.mipmap_level+1);
				{
					vk::BufferImageCopy region{};
					region.bufferOffset = 0;
					region.bufferRowLength = 0;
					region.bufferImageHeight = 0;

					region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					region.imageSubresource.mipLevel = 0;// std::max(load_info.mipmap_level, 1u) - 1;
					region.imageSubresource.baseArrayLayer = 0;
					region.imageSubresource.layerCount = 1;

					region.imageOffset = { 0, 0, 0 };
					region.imageExtent = {
						width      ,
						height ,
						1
					};
					copy_regions[0] = (region);
				}
				for (uint32_t i = 1; i <= load_info.mipmap_level; ++i)
				{

				vk::BufferImageCopy region{};
				region.bufferOffset = 0;
				region.bufferRowLength = 0;
				region.bufferImageHeight = 0;

				region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
				region.imageSubresource.mipLevel = i;// std::max(load_info.mipmap_level, 1u) - 1;
				region.imageSubresource.baseArrayLayer = 0;
				region.imageSubresource.layerCount = 1;

				region.imageOffset = { 0, 0, 0 };
				region.imageExtent = {
					width>>i,
					height>>i,
					1
				};
				copy_regions[i]=(region);
				}
				//vector<vk::BufferImageCopy>

				cmd_buffer.copyBufferToImage(*stagingBuffer, copy_dest, vk::ImageLayout::eTransferDstOptimal, copy_regions, vk::DispatchLoaderDefault{});

				staging_buffer = std::move(stagingBuffer);
				staging_memory = std::move(stagingMemory);
				//TransitionImageLayout(cmd_buffer, src_flags, shader_flags, dst_flags, dst_stages, vk::ImageLayout::eTransferDstOptimal, layout, copy_dest);
				;
			}


			if (internal_format != format)
			{
				TransitionImageLayout(cmd_buffer, vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eTransferRead, vk::PipelineStageFlagBits::eTransfer, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, copy_dest, img_aspect,sub_range);
				TransitionImageLayout(cmd_buffer, {}, vk::PipelineStageFlagBits::eAllCommands , vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTransfer, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, * image, img_aspect, sub_range);
				BlitConvert(cmd_buffer, img_aspect, *blit_src_img, *image, load_info.mipmap_level, width, height);
			}
			TransitionImageLayout(cmd_buffer, vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTransfer, dst_flags, dst_stages, vk::ImageLayout::eTransferDstOptimal, load_info.layout, *image, img_aspect);

	
		}else if(imageInfo.initialLayout!=load_info.layout)
		{
			TransitionImageLayout(cmd_buffer, vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTransfer, {}, vk::PipelineStageFlagBits::eAllCommands, vk::ImageLayout::eUndefined, load_info.layout, * image, img_aspect);
		}
		device.resetFences(fence);
		hlp::EndSingleTimeCbufferCmd(cmd_buffer, view.GraphicsQueue(), false, fence);
		uint64_t wait_for_milli_seconds = 1;
		[[maybe_unused]] uint64_t wait_for_micro_seconds = wait_for_milli_seconds * 0;
		//uint64_t wait_for_nano_seconds = wait_for_micro_seconds * 1000;
		while (device.waitForFences(fence, VK_TRUE, wait_for_milli_seconds) == vk::Result::eTimeout);
		result.first = std::move(image);
		result.second = std::move(alloc);
		return std::move(result);//std::pair<vk::UniqueImage, hlp::UniqueAlloc>{, };
	}
	bool fml = false;
	bool IsDepthStencil(vk::Format format);
	void Mark() { fml = true; }
#pragma optimize("",off)
	TextureResult LoadTexture(hlp::MemoryAllocator& allocator, vk::Fence fence, const void* data, uint32_t width, uint32_t height, size_t len, vk::Format format, bool is_render_target)
	{
		TextureResult result;
		VulkanView& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		vk::PhysicalDevice pd = view.PDevice();
		vk::Device device = *view.Device();
		auto ucmd_buffer = hlp::BeginSingleTimeCBufferCmd(device, *view.Commandpool());
		auto cmd_buffer = *ucmd_buffer;
		//bool is_render_target = isRenderTarget;

		if (len == 0 && !data) //If data isn't given.
			len = ComputeTextureLength(width, height, format);

		size_t num_bytes = len;

		vk::ImageUsageFlags attachment_type = IsDepthStencil(format)? vk::ImageUsageFlagBits::eDepthStencilAttachment:vk::ImageUsageFlagBits::eColorAttachment;
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
		if (IsDepthStencil(format ))
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
	TextureOptions::TextureOptions(const CompiledTexture& meta)
	{
		min_filter = mag_filter = filter_mode = meta.filter_mode;
		uv_mode = meta.mode;
		internal_format = ToInternalFormat(meta.internal_format, meta.is_srgb);
	}
}