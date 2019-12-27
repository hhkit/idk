#pragma once
#include <res/Guid.h>
#include <res/ResourceMeta.h>
#include <res/ResourceHandle.h>
#include <meta/comparator.h>
namespace idk
{
	struct ResourceBundle;

	struct MetaBundle
		: comparable<MetaBundle>
	{
		vector<SerializedMeta> metadatas;

		template<typename T>
		void Add(RscHandle<T>);

		const SerializedMeta* FetchMeta(string_view name) const;

		template<typename T>
		const SerializedMeta* FetchMeta() const;

		template<typename T>
		const SerializedMeta* FetchMeta(string_view name) const;

		explicit operator bool() const;

		template<typename FullResType> RscHandle<FullResType> CreateResource() const;
		template<typename FullResType> RscHandle<FullResType> CreateResource(string_view name) const;

		bool operator<(const MetaBundle&) const;
	};
}
