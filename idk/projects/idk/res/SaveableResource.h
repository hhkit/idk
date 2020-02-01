#pragma once
#include <idk.h>
#include <res/ResourceExtension.h>


namespace idk
{
	namespace detail {
		template<typename T> struct ResourceManager_detail;
		template<typename T> struct CompiledAssetHelper_detail;
	}
	template<typename T, typename AutoSave = true_type>
	struct Saveable
	{
		static constexpr auto autosave = AutoSave::value;

		bool IsDirty() const { return _dirty; }
		void Dirty() { if (_saveable) _dirty = true; };
		void SetSaveableFlag(bool set) { _saveable = set; _dirty = false; }
	private:
		template<typename T>
		friend struct detail::ResourceManager_detail;
		friend class ResourceManager;

		bool _dirty = false;
		bool _saveable = true;

		void Clean() { _dirty = false; };
	};
}