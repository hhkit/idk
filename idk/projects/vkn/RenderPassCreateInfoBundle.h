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
			span<vk::SubpassDescription> subpass_desc,
			span<vk::SubpassDependency> subpass_dep)
		{
			auto n_dep = std::min(std::size(_data->_subpass_dep), subpass_dep.size());
			for (size_t i = 0; i < n_dep; ++i)
			{
				_data->_subpass_dep[i] = subpass_dep[i];
			}
			_data->_subpass_deps = { _data->_subpass_dep ,_data->_subpass_dep + n_dep };

			auto n_desc = std::min(std::size(_data->_subpass_desc), subpass_desc.size());
			for (size_t i = 0; i < n_desc; ++i)
			{
				_data->_subpass_desc[i] = subpass_desc[i];
			}
			_data->_subpass_descs = { _data->_subpass_desc ,_data->_subpass_desc + n_desc };


			_rpci = vk::RenderPassCreateInfo
			{
				vk::RenderPassCreateFlags{},
				static_cast<uint32_t>(attachments().size()),
				std::data(attachments()),
				static_cast<uint32_t>(_data->_subpass_descs.size()),_data->_subpass_descs.data(),
				static_cast<uint32_t>(_data->_subpass_deps.size() ),_data->_subpass_deps.data()
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
			vk::SubpassDescription  _subpass_desc[5]
			{};
			span< vk::SubpassDescription> _subpass_descs;

			vk::SubpassDependency  _subpass_dep[5]
			{};
			span< vk::SubpassDependency> _subpass_deps;
		};
		std::unique_ptr<Data> _data;
		vk::RenderPassCreateInfo _rpci
		{};
	};
}