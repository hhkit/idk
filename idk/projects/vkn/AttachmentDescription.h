#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/VknTexture.h>
namespace idk::vkn
{

	struct TextureDescription
	{
		string                               name          ;
		uvec2                                size          ;
		vk::Format                           format        ;
		vk::ImageAspectFlagBits              aspect        ;
		vk::ImageType                        type          = vk::ImageType::e2D;
		uint32_t                             layer_count   = 1;
		uint32_t                             mipmap_level  = 1;
		vk::ImageTiling                      tiling_format = vk::ImageTiling::eOptimal;
		vk::ImageUsageFlags                  usage         {};
		std::optional<RscHandle<VknTexture>> actual_rsc    ;
		static TextureDescription make(
			std::optional<string                 >             _name          = {},
			std::optional<uvec2                  >             _size          = {},
			std::optional<vk::Format             >             _format        = {},
			std::optional<vk::ImageAspectFlagBits>             _aspect        = {},
			std::optional<vk::ImageType          >             _type          = {},
			std::optional<uint32_t               >             _layer_count   = {},
			std::optional<uint32_t               >             _mipmap_level  = {},
			std::optional<vk::ImageTiling        >             _tiling_format = {},
			std::optional<vk::ImageUsageFlags    >             _usage         = {},
			std::optional<RscHandle<VknTexture>  >             _actual_rsc    = {}
		)
		{
			TextureDescription td{};
			if(_name          ) td.name          = *_name          ;
			if(_size          ) td.size          = *_size          ;
			if(_format        ) td.format        = *_format        ;
			if(_aspect        ) td.aspect        = *_aspect        ;
			if(_type          ) td.type          = *_type          ;
			if(_layer_count   ) td.layer_count   = *_layer_count   ;
			if(_mipmap_level  ) td.mipmap_level  = *_mipmap_level  ;
			if(_tiling_format ) td.tiling_format = *_tiling_format ;
			if(_usage         ) td.usage         = *_usage         ;
			if(_actual_rsc    ) td.actual_rsc    = *_actual_rsc    ;
			return td;
		}
		static TextureDescription make(string _name, vk::ImageUsageFlags _usage, RscHandle<VknTexture> _actual_rsc)
		{
			return TextureDescription{
				.name        { std::move(_name) },
				.size        { _actual_rsc->Size() },
				.format      { _actual_rsc->format },
				.aspect      { _actual_rsc->img_aspect },
				.type        { _actual_rsc->image_type },
				.layer_count { _actual_rsc->Layers() },
				.mipmap_level{ _actual_rsc->mipmap_level },
				.usage       { _usage },
				.actual_rsc  { _actual_rsc }
			};
		}
		static TextureDescription make(string _name, vk::ImageUsageFlags _usage, RscHandle<Texture> _actual_rsc)
		{
			return make(std::move(_name), _usage, RscHandle<VknTexture>{_actual_rsc});
		}
	};
	struct AttachmentDescription
	{
		vk::AttachmentLoadOp          load_op            ;
		vk::AttachmentStoreOp         store_op           ;
		vk::AttachmentLoadOp          stencil_load_op    ;
		vk::AttachmentStoreOp         stencil_store_op   ;
		vk::ImageLayout               layout             {vk::ImageLayout::eGeneral}; //layout after RenderPass
		vk::ImageSubresourceRange     sub_resource_range {};
		std::optional<vk::ClearValue> clear_value        ;
		std::optional<vk::Format>     format             {};
		vk::ImageViewType             view_type          { vk::ImageViewType::e2D };
		vk::ComponentMapping          mapping            {};
		static AttachmentDescription make(
			vk::AttachmentLoadOp              load_op             = {},
			vk::AttachmentStoreOp             store_op            = {},
			vk::AttachmentLoadOp              stencil_load_op     = {},
			vk::AttachmentStoreOp             stencil_store_op    = {},
			vk::ImageSubresourceRange         sub_resource_range  = {},
			std::optional<vk::ClearValue>     clear_value         = {},
			std::optional<vk::Format>         format              = {},
			vk::ComponentMapping              mapping             = {},
			std::optional<vk::ImageLayout  >  layout              = {},
			std::optional<vk::ImageViewType>  view_type           = {}
		)
		{
			AttachmentDescription def{};
			return
				AttachmentDescription
			{
				load_op                                 ,
				store_op                                ,
				stencil_load_op                         ,
				stencil_store_op                        ,
				(layout) ? *layout : def.layout         ,
				sub_resource_range                      ,
				clear_value                             ,
				format                                  ,
				(view_type) ? *view_type : def.view_type,
				mapping                                 ,
			};
		}
	};
	
}