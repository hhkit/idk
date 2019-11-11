#pragma once

#include <idk.h>

namespace idk {
	struct FontPoint {
		real x{};
		real y{};
		real s{};
		real t{};
	};
	struct FontData {
		vector<FontPoint> coords;
		color color{1.f,1.f ,1.f ,1.f };
		RscHandle<FontAtlas> fontAtlas;
		//RscHandle<Mesh> fontMesh;
		mat4 transform;
		int n_size{0};
	};
}