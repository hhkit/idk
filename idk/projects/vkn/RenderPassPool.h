#pragma once
#include <idk.h>

#include <vulkan/vulkan.hpp>
#include <vkn/RenderPassObj.h>

namespace idk::vkn
{






	class RenderPassPool
	{
	public:
		//using rp_hash_t = size_t;
		using rp_type_t = string;

		using rp_info_t = vk::RenderPassCreateInfo;

		//TODO
		void Reset();//To reset the allocation tracker and free up the unused renderpasses

		RenderPassObj GetRenderPass(const rp_info_t& rpci)
		{
			RenderPassObj result = {};
			auto type = ComputeType(rpci);
			auto itr = _subpools.find(type);

			result = (itr == _subpools.end()) ? CreateRenderPass(type, rpci) : itr->second;
			return result;
		}

	private:

		hash_table<rp_type_t, RenderPassObj> _subpools;

		//rp_hash_t ComputeHash(const rp_info_t& rpci)const;
		rp_type_t ComputeType(const rp_info_t& rpci)const;
		RenderPassObj CreateRenderPass(rp_type_t type, const rp_info_t& rpci);
	};
	   	 

	class FramebufferPool
	{
	public:
		//using fb_hash_t = size_t;
		using fb_type_t = string;

		using fb_info_t = vk::FramebufferCreateInfo;

		//TODO
		void Reset();//To reset the allocation tracker and free up the unused Framebufferes

		vk::Framebuffer GetFramebuffer(const fb_info_t& fbci)
		{
			vk::Framebuffer result = {};
			auto type = ComputeType(fbci);
			auto itr = _subpools.find(type);

			result = (itr == _subpools.end()) ? CreateFramebuffer(type, fbci) : *itr->second;
			return result;
		}

	private:

		hash_table<fb_type_t, vk::UniqueFramebuffer> _subpools;

		//fb_hash_t ComputeHash(const fb_info_t& fbci)const;
		fb_type_t ComputeType(const fb_info_t& fbci)const;
		vk::Framebuffer CreateFramebuffer(fb_type_t type, const fb_info_t& fbci);
	};





}