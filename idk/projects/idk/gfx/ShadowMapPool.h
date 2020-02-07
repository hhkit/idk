#pragma once

#include <gfx/Lightmap.h>
#include <gfx/LightTypes.h>

namespace idk
{
	namespace detail
	{
		template<typename V>
		struct variant_count;
		template<typename ... Args>
		struct variant_count< std::variant <Args...>> { constexpr static inline size_t value = sizeof...(Args); };

		template<typename V>
		constexpr static inline size_t variant_count_v = variant_count<V>::value;
	}

	struct ShadowMapPool
	{
		ShadowMapPool() = default;
		ShadowMapPool(const ShadowMapPool&) = delete;
		ShadowMapPool(ShadowMapPool&&) = default;
		ShadowMapPool& operator=(const ShadowMapPool&) = delete;
		ShadowMapPool& operator=(ShadowMapPool&&) = default;
		~ShadowMapPool();

		void Restart();
		vector<Lightmap> GetShadowMaps(const Light& light);
		vector<Lightmap> GetShadowMaps(size_t light_index,const vector<Lightmap>& to_dup);

	private:
		//Maybe change to span
		struct Entry
		{
			//uvec2 size;
			vector<Lightmap> shadows;
		};

		struct Subpool
		{
			size_t next;
			vector<Entry> entries;
			void grow(const vector<Lightmap>&);
			void reset();
			vector<Lightmap> Get(const vector<Lightmap>&);
		};

		std::array<Subpool,detail::variant_count_v<LightVariant>> _lightmaps;
	};
}