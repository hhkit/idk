#pragma once
#include "IAsyncLoader.h"
#include <vector>
#include <optional>
#include <mutex>
#include <parallel/ThreadPool.h>
namespace idk::vkn
{

	template<typename Entry>
	struct AsyncLoaderDefaultTraits
	{
		static auto& GetResourceHandle(Entry& entry)
		{
			return entry.handle;
		}
		static void FlagAsReady(Entry& entry)
		{
			entry.handle->use_default(false);
		}
	};
	template<typename entry_t, typename Traits = AsyncLoaderDefaultTraits<entry_t>>
	class AsyncLoader : public IAsyncLoader
	{
	public:
		using Entry = entry_t;
		virtual void ProcessEntry(Entry& entry) = 0;
		size_t NumAdded()const override
		{
			return queued_counter_;
		}
		size_t NumProcessed()const override
		{
			return processed_counter_;
		}
		void Add(Entry&& entry)
		{
			std::lock_guard lock{ pending_mutex };
			pending_stack.emplace_back(std::move(entry));
			++queued_counter_;
		}
		void FlagProcessedAsReady()
		{
			for (auto& entry : process_list)
			{
				Traits::FlagAsReady(entry);
			}
		}
		void Update()override
		{
			if (proc && proc->ready())
			{
				proc->get();
				proc.reset();
				FlagProcessedAsReady();
			}
			if (!proc) //Not processing anything right now.
			{
				SwapBuffers();//Clear front and swap with back
				if (!process_list.empty()) //If there's something to process, create job to do it
				{
					proc = Core::GetThreadPool().Post([this]() {
						this->Process();
						});
				}
			}
		}
		void Process()
		{
			for (auto& entry : process_list)
			{
				ProcessEntry(entry);
				++processed_counter_;
			}
		}
		void SwapBuffers()
		{
			process_list.clear();
			std::lock_guard lock{ pending_mutex };//Ensure no one is modifying pending_stack
			std::swap(pending_stack, process_list);
		}
	private:
		size_t queued_counter_{};//Guarded by pending_mutex
		size_t processed_counter_{};
		std::vector<Entry> process_list;
		std::mutex pending_mutex;
		std::vector<Entry> pending_stack;
		std::optional<mt::Future<void>> proc;

	};

}