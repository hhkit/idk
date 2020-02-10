#pragma once
#include "idk.h"

#include "Lightmap.h"


namespace idk {

	class CascadedSet {
	public:
		CascadedSet() = default;
		CascadedSet(CameraData camData);
		void Setup(CameraData camData);

		vector<Lightmap> cascadeSet{};
		int cascade_count = CASCADE_COUNT;
	private:
		bool needSetup = true;
	};
}