#pragma once
#include <memory>
#include <vector>
#include "IAsyncLoader.h"
namespace idk::vkn
{
	class AsyncLoaders
	{
	public:
		std::string future_errs;
		void AddLoader(std::weak_ptr<IAsyncLoader> loader)
		{
			loaders_.emplace_back(loader);
		}
		void UpdateLoaders()
		{
			invalid_indices_.clear();
			size_t i = 0;
			for (auto& wloader : loaders_)
			{
				if (auto loader = wloader.lock())
				{
					loader->Update();
				}
				else
				{
					invalid_indices_.emplace_back(i);
				}
				++i;
			}
			while (!invalid_indices_.empty())
			{
				loaders_.erase(loaders_.begin() + invalid_indices_.back());
				invalid_indices_.pop_back();
			}
			future_errs.clear();
			for (auto& wloader : loaders_)
			{
				future_errs += wloader.lock()->future_err + "\n";
			}
		}
		void ResetCounters()
		{

			for (auto& wptr : loaders_)
			{
				if (auto ptr = wptr.lock())
				{
					ptr->ClearAndResetCounters();
				}
			}
		}
		size_t NumAdded()const
		{
			size_t total = 0;
			for (auto& wptr : loaders_)
			{
				if (auto ptr = wptr.lock())
				{
					total += ptr->NumAdded();
				}
			}
			return total;
		}
		size_t NumProcessed()
		{
			size_t total = 0;
			for (auto& wptr : loaders_)
			{
				if (auto ptr = wptr.lock())
				{
					total += ptr->NumProcessed();
				}
			}
			return total;

		}
	private:
		std::vector<std::weak_ptr<IAsyncLoader>> loaders_;
		std::vector<size_t> invalid_indices_;
	};
}