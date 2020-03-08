#pragma once
#include <yojimbo/yojimbo.h>

#define serialize_vector_count(stream, vec, max_size) \
{                                                     \
	auto count = vec.size();                          \
	serialize_int(stream, count, 0, max_size);        \
	vec.resize(count);                                \
}

#define serialize_dataonly_struct(stream, obj) \
	serialize_bytes(stream, (uint8_t*) &obj, sizeof(obj))

#define serialize_vec(stream, vec)      \
{                                       \
	for (auto& coord : vec)             \
		serialize_float(stream, coord); \
}

namespace idk
{

}