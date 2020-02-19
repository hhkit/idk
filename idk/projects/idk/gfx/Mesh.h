#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <gfx/vertex_descriptor.h>
#include <math/shapes/sphere.h>
#include <res/Guid.inl>

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
		, INV_FSQ
		, DbgBox
		, DbgSphere // No horizon disk
		, DbgArrow  // Cone shape (used for ray)
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
			{ Guid{0xDCC694FE, 0x4284, 0x4273, 0x80041523C5805BEE} }, // INV_FSQ
			{ Guid{0x853718BE, 0xDFBB, 0x4B57, 0xBFB6F339C348AEF2} }, // Debug Cube
			{ Guid{0x2BBC493A, 0xB5FB, 0x4770, 0xBDE354837C0EB100} }, // Debug sphere
			{ Guid{0x526A8C26, 0x6BD2, 0x4E25, 0xBFECC29C83FB6E89} }  // Debug arrow
		};
		sphere bounding_volume;
		virtual int GetAttribs() const = 0;
		virtual ~Mesh() = default;
	};
}