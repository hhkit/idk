#pragma once
#include <idk.h>
#include <res/ResourceExtension.h>

namespace idk
{
	template<typename T, typename AutoSave = true_type>
	struct Saveable
	{
		static constexpr auto autosave = AutoSave::value;

		bool IsDirty() const { return _dirty; }
		void Dirty() { _dirty = true; };
	private:
		template<typename T>
		friend struct detail::ResourceManager_detail;
		friend class ResourceManager;

		bool _dirty = false;

		void Clean() { _dirty = false; };
	};
}