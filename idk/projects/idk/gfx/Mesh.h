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
	);

	class Mesh
		: public Resource<Mesh>
	{
	public:
		static constexpr RscHandle<Mesh> defaults[MeshType::count] =
		{
			{ Guid{0xF66BD34D, 0x05C847A5, 0xB4F14AD7, 0x06DDA0EC} }, // Box
			{ Guid{0x1ADE959C, 0xCA814653, 0x8E8109DF, 0xCDC81D22} }, // Sphere
			{ Guid{0x53B3895A, 0x0C8E4086, 0xA24533AF, 0x707B891E} }, // Tetrahedron
			{ Guid{0x1567AC7E, 0x329443AA, 0x959302FE, 0xC33E7366} }, // Line
			{ Guid{0xBE85068A, 0x621443FA, 0xA59ABC2F, 0x552DA7F3} }, // Circle
		};

		virtual int GetAttribs() const = 0;
		virtual ~Mesh() = default;
	};
}