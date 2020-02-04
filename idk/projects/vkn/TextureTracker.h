#pragma once
#include <idk.h>
#include <ds/lazy_vector.h>
namespace idk::vkn::dbg
{
	enum class TextureAllocTypes : int
	{
		eTexture=0,
		eCubemap,
		eFontAtlas
	};

	class TextureTracker
	{
	public:
		struct entry
		{
			size_t alloc_id = {};
			size_t size = {};
		};
		void reg_allocate(void* handle, size_t sz);
		void reg_deallocate(void* handle);
		static const lazy_vector<TextureTracker>& Insts() { return _Insts(); }
		static TextureTracker& Inst(TextureAllocTypes = TextureAllocTypes::eTexture);
		hash_table<void*, entry> dump()const;
	private:
		static lazy_vector<TextureTracker>& _Insts();
		size_t _alloc_id = {};
		hash_table<void*, entry> _allocs;
	};
}