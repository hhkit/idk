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
		void ClearAndResetCounters()override
		{
			queued_counter_ = 0;
			processed_counter_ = 0;
			std::lock_guard lock{ pending_mutex };
			pending_stack.clear();
			process_list.clear();
		}
		void Add(Entry&& entry)
		{
			std::lock_guard lock{ pending_mutex };
			pending_stack.emplace_back(std::move(entry));
			++queued_counter_;
		}
		void FlagProcessedAsReady()
		{
			size_t i = 0;
			for (auto& entry : process_list)
			{
				if (processed_flags_.at(i))
				{
					Traits::FlagAsReady(entry);
					processed_flags_.at(i) = false;//mark it false again to prevent double ready setting
				}
				++i;
			}
		}
		void Update()override
		{
			if (proc && proc->ready())
			{
				FlagProcessedAsReady();
				proc->get();
				proc.reset();
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
			size_t i = 0;
			for (auto& entry : process_list)
			{
				ProcessEntry(entry);
				processed_flags_[i] = true;
				++i;
				++processed_counter_;
			}
		}
		void SwapBuffers()
		{
			process_list.clear();
			processed_flags_.clear();
			{
			std::lock_guard lock{ pending_mutex };//Ensure no one is modifying pending_stack
			std::swap(pending_stack, process_list);
			}
			processed_flags_.resize(process_list.size(), false);
		}
	private:
		size_t queued_counter_{};//Guarded by pending_mutex
		size_t processed_counter_{};
		std::vector<Entry> process_list;
		std::vector<bool> processed_flags_;
		std::mutex pending_mutex;
		std::vector<Entry> pending_stack;
		std::optional<mt::Future<void>> proc;

	};

}