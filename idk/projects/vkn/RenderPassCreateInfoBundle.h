#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
namespace idk::vkn
{

	struct RenderPassCreateInfoBundle
	{
		auto& attachments()            { return _data->_attachments           ; }
		auto& attachment_input_refs()  { return _data->_attachment_input_refs ; }
		auto& attachment_output_refs() { return _data->_attachment_output_refs; }
		auto& depth_ref()              { return _data->_depth_ref             ; }

		operator vk::RenderPassCreateInfo()const
		{
			return _rpci;
		}
		void set(
			vk::SubpassDescription subpass_desc,
			vk::SubpassDependency subpass_dep)
		{
			_data->_subpass_dep = subpass_dep;
			_data->_subpass_desc = subpass_desc;

			_rpci = vk::RenderPassCreateInfo
			{
				vk::RenderPassCreateFlags{},
				static_cast<uint32_t>(attachments().size()),
				std::data(attachments()),
				1,& _data->_subpass_desc,
				1,& _data->_subpass_dep
			};
		}
		RenderPassCreateInfoBundle() :_data{ std::make_unique<Data>() } {}		
	private:
		struct Data
		{
			vector<vk::AttachmentDescription> _attachments{};
			vector<vk::AttachmentReference>   _attachment_input_refs{};
			vector<vk::AttachmentReference>   _attachment_output_refs{};
			vk::AttachmentReference           _depth_ref{};
			vk::SubpassDescription _subpass_desc{};
			vk::SubpassDependency  _subpass_dep
			{};
		};
		std::unique_ptr<Data> _data;
		vk::RenderPassCreateInfo _rpci
		{};
	};
}