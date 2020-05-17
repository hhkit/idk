#pragma once
#include <idk.h>
#include <vkn/MemoryCollator.h>
#include <gfx/MaterialInstance.h>

#include <res/ResourceHandle.h>

#include <ds/span.inl>

#include <vkn/UniformManager.h>
#include <vkn/UboManager.h>

//#include <vkn/GraphicsState.h>

#include <vkn/MemoryAllocator.h>


namespace idk::vkn
{
	struct ProcessedMaterial;
	struct BindingData;
	struct DsUpdater
	{
		void associate(vk::DescriptorSet ds, const BindingData& binding);

		void UpdateDescriptorSets();

		vector<vk::WriteDescriptorSet> write_buffer;
		vector<vk::DescriptorBufferInfo> b_scratch;
		vector<vk::DescriptorImageInfo>  i_scratch;

	};

	class MaterialInstanceCache
	{
	public:
		using set_t = uint32_t;
		using descriptors_t = std::tuple<uint32_t, vk::DescriptorSetLayout, vk::DescriptorSet>;
		struct InstCachedInfo;
		using creation_buffer_t = vector < std::pair<InstCachedInfo&, const  ProcessedMaterial&>>;
		struct SetCachedInfo
		{
			size_t index = 0;
			vector<RscHandle<Texture>> tex;
			using binding_t = uint32_t;
			bool dirty = true;
		};

		struct UpdateInfo;

		struct InstCachedInfo
		{
			hash_table<set_t, SetCachedInfo> sets;
			vk::ShaderModule frag_shader = {};

			struct BufferInfo
			{
				vk::Buffer buffer;
				index_span buffer_range;
				index_span data_range;
			};
			
			vector<BufferInfo> buffers;

			size_t data_hash = 0;
			size_t texture_hash = 0;

			string data_cache;
			string texture_cache;
			string scratch_data_cache;
			string scratch_texture_cache;
			vector<vk::Image> image_cache;

			vector< descriptors_t> descriptors;
			//indexes into descriptors
			hash_table<set_t, size_t> descriptor_indices;
			void Update(const ProcessedMaterial& mat_inst, UpdateInfo& update_info);
			void ReleaseDescriptorSets(DescriptorsManager& dm);
		};

		void Start();
		void CacheMaterialInstance(const ProcessedMaterial& inst);
		//Does not check if inst is invalid
		bool IsCached(RscHandle<MaterialInstance> mat_inst)const;
		span<const descriptors_t> GetDescriptorSets(RscHandle<MaterialInstance> inst)const;
		void UpdateUniformBuffers();
		void End();
		void ProcessCreation();

		MaterialInstanceCache();
		~MaterialInstanceCache();
	private:
		struct PImpl;
		std::unique_ptr<PImpl> _pimpl;
		hash_table<RscHandle<MaterialInstance>, InstCachedInfo> cached_info;
	};
}