#pragma once
#include <idk.h>
#include <gfx/Mesh.h>
#include <vulkan/vulkan.hpp>
#include <gfx/vertex_descriptor.h>

namespace idk::vkn
{
	enum class BufferType
	{
		eAttrib,
		eIndex
	};
	using attrib_index = BufferType;
	struct MeshBuffer
	{
		vk::UniqueBuffer buffer{};
		size_t len{};
	};
	class VulkanMesh : public Mesh
	{
	public:
		using buffers_t = hash_table<attrib_index ,MeshBuffer>;
		const MeshBuffer& Get(attrib_index index)const 
		{
			auto itr = buffers.find(index);
			if(itr!=buffers.end())
				return itr->second;
			return MeshBuffer{};
		}
		const buffers_t& Buffers()const { return buffers; }
		int GetAttribs() const override;
		void SetBuffer(attrib_index type,MeshBuffer&& buffer)
		{
			buffers[type]=std::move(buffer);
		}
	private:
		buffers_t buffers{};
	};

}
