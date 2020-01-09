#pragma once
#include <idk.h>

#define CASCADE_COUNT 3


namespace idk {
	class Cascade {

	public:
		Cascade();
	private:
		mat4 matrix;
	};
};