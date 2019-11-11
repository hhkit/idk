#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <gfx/vertex_descriptor.h>

namespace idk
{
	ENUM(MeshType, int
		, Box
		, Sphere
		, Tetrahedron
		, Line
		, Circle
		, Plane
		, FSQ
	); // FSQ stands for Full Screen Quad

	class Mesh
		: public Resource<Mesh>
	{
	public:
		static constexpr RscHandle<Mesh> defaults[MeshType::count] =
		{
			{ Guid{0xF66BD34D, 0x11E9, 0x47A5, 0xB4F14AD706DDA0EC} }, // Box
            { Guid{0x1ADE959C, 0x5194, 0x4653, 0x8E8109DFCDC81D22} }, // Sphere
            { Guid{0x53B3895A, 0x9021, 0x4086, 0xA24533AF707B891E} }, // Tetrahedron
            { Guid{0x1567AC7E, 0x10EA, 0x43AA, 0x959302FEC33E7366} }, // Line
            { Guid{0xBE85068A, 0x10FE, 0x43FA, 0xA59ABC2F552DA7F3} }, // Circle
            { Guid{0x5EACF9F1, 0x9036, 0x40DA, 0xA7356B51E5294654} }, // Plane
            { Guid{0x5D6C8AD5, 0x927D, 0x49F7, 0x9B619AE892A96A3F} }, // FSQ
		};
		sphere bounding_volume;
		virtual int GetAttribs() const = 0;
		virtual ~Mesh() = default;
	};
}