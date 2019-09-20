#pragma once
#include <vulkan/vulkan.hpp>

#include <vkn/utils/TriBuffer.h>


namespace idk::vkn {

	struct TriBuffer;
	struct SwapChainInfo
	{
		uint32_t curr_index{};
		vk::UniqueSwapchainKHR             swap_chain;
		vk::PresentModeKHR				   present_mode;
		vk::SurfaceFormatKHR			   surface_format;
		//vk::Format                         format;
		vk::Extent2D                       extent;

		TriBuffer							m_graphics;
		vector<shared_ptr<TriBuffer>>       m_inBetweens;
		TriBuffer							m_swapchainGraphics;

		std::vector<vk::UniqueFramebuffer> frame_buffers;

		std::vector<std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory>> uniform_buffers;
		std::vector<std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory>> uniform_buffers2;
		//TODO FrameData
		//class FrameData
		//Stores data in frame (images, image_views, framebuffers for swapping)

		std::vector<vk::DescriptorSet    > descriptor_sets;
		std::vector<vk::DescriptorSet    > descriptor_sets2;

		vector<FrameObjects> frame_objects;

		struct UniformStuff
		{
			std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory> uniform_buffer;
			vk::DescriptorSet descriptor_set;

		};
		struct Uniforms
		{
			vk::UniqueDescriptorSetLayout        layout;

			vector<UniformStuff> frame;
			void Init(size_t num) { frame.resize(num); }
			auto& uniform_buffer(size_t index) { return frame[index].uniform_buffer; }
			auto& descriptor_set(size_t index) { return frame[index].descriptor_set; }

			decltype(frame)::iterator begin() { return frame.begin(); }
			decltype(frame)::iterator end() { return frame.end(); }
			uint32_t size() { return s_cast<uint32_t>(frame.size()); }
		};
		//struct UniformManager
		//{
		//	struct BindingInfo
		//	{
		//		uint32_t offset;
		//		uint32_t size;
		//	};
		//	hlp::vector_buffer master_bo;
		//	string             local_buffer;
		//	vector<BindingInfo> bindings;
		//};
		Uniforms uniforms2;
	};
};