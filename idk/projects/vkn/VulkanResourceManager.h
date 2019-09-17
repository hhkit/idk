#pragma once
#include <idk.h>

#include <vulkan/vulkan.hpp>
namespace idk::vkn
{
	class VulkanView;
	class VulkanWin32GraphicsSystem;
	class VulkanResourceManager;

	struct VulkanRscBase
	{
		VulkanRscBase() = default;
		VulkanRscBase(VulkanRscBase&&)noexcept = default;
		//destroys/releases the object immediately (it is possible for this function to not be called before destroying via destructor)
		virtual void Destroy()=0;
		virtual ~VulkanRscBase()=default;
	};
	template<typename T>
	struct VulkanRscDel : VulkanRscBase,vk::UniqueHandle<T, vk::DispatchLoaderDefault>
	{
		using Base = vk::UniqueHandle<T, vk::DispatchLoaderDefault>;
		using Base::Base;
		VulkanRscDel(Base&& base)noexcept :Base{ std::move(base) } {}
		VulkanRscDel& operator=(VulkanRscDel&& rhs)
		{
			auto& tmp = s_cast<Base&>(*this);
			std::swap(tmp, s_cast<Base&>(rhs));
			return *this;
		}
		VulkanRscDel& operator=(Base&& rhs)
		{
			*this = VulkanRscDel{ std::move(rhs) };
			return *this;
		}
		void Destroy() override
		{
			this->reset();
		}
	};
	namespace impl
	{
		VulkanResourceManager* GetRscManager();
	}
	template<typename T>
	struct VulkanRsc : vk::UniqueHandle<T, vk::DispatchLoaderDefault>
	{
		using Base = vk::UniqueHandle<T, vk::DispatchLoaderDefault>;
		using Base::Base;
		VulkanRsc() = default;
		VulkanRsc(Base&& base)noexcept :Base{ std::move(base) } {}

		VulkanRsc& operator=(VulkanRsc&& rhs)
		{
			auto& tmp = s_cast<Base&>(*this);
			std::swap(tmp, s_cast<Base&>(rhs));
			return *this;
		}
		VulkanRsc& operator=(Base&& rhs)
		{
			*this = VulkanRsc{ std::move(rhs) };
			return *this;
		}

		//void Destroy() override
		//{
		//	reset();
		//}
		~VulkanRsc();
	};
	class VulkanResourceManager
	{
	public:
		using ptr_t = unique_ptr<VulkanRscBase>;
		/*void RegisterRsc(shared_ptr<VulkanRscBase> rsc)
		{
			managed.emplace_back(rsc);
		}*/
		void QueueToDestroy(ptr_t obj_to_destroy);
		void ProcessQueue(uint32_t frame_index);
		/*
		//Finds and queues all the resources with no existing references.
		void ProcessSingles(uint32_t frame_index)
		{
			auto& queue = destroy_queue[frame_index];
			//Destroy the stuff that have already waited for 1 full frame cycle
			queue.clear();//Assumes that the shared_ptrs queued are ready for destruction
			auto itr = managed.begin(), end = managed.end();
			for (; itr < end; ++itr)
			{
				//Last owner, assume no weak_ptrs to be locked in another thread at this time
				if (itr->use_count() == 1)
				{
					//swap to "back"
					std::swap(*itr, (*--end));
				}
			}
			//Queue the next batch of stuff to be destroyed.
			queue.insert(queue.end(), end, managed.end());
			managed.resize(end - managed.begin());//trim it down
		}
		*/
		void DestroyAll();
	private:
		vector<ptr_t> managed;
		hash_table<uint32_t, vector<ptr_t>> destroy_queue;
	};

	template<typename T>
	VulkanRsc<T>::~VulkanRsc()
	{
		//If valid pointer
		if (*this)
		{
			VulkanResourceManager* manager = impl::GetRscManager();
			manager->QueueToDestroy(std::make_unique<VulkanRscDel<T>>(s_cast<Base&&>(std::move(*this))));

		}
	}

}