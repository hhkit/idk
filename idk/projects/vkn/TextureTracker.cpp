#include "pch.h"
#include "TextureTracker.h"
#include <ds/lazy_vector.h>
namespace idk::vkn::dbg
{
	constexpr inline bool enabled = false;

	void TextureTracker::reg_allocate(void* handle, size_t sz)
	{
		if constexpr (enabled)
		{
			_allocs.emplace(handle, entry{ _alloc_id++,sz });
		}
	}

	void TextureTracker::reg_deallocate(void* handle)
	{
		if constexpr (enabled)
		{
			auto itr = _allocs.find(handle);
			if (itr != _allocs.end())
			{
				_allocs.erase(itr);
			}
		}
	}

	lazy_vector<TextureTracker>& TextureTracker::_Insts()
	{
		static lazy_vector<TextureTracker> tt{};
		return tt;
	}

	TextureTracker& TextureTracker::Inst(TextureAllocTypes category)
	{
		if constexpr (enabled)
		{
			return _Insts()[static_cast<uint32_t>(category)];
		}
		else
		{
			static TextureTracker t;
			return t;
		}
	}

	hash_table<void*, TextureTracker::entry> TextureTracker::dump() const
	{
		return _allocs;
	}

}