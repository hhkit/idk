#pragma once
#include <idk.h>
#include <math/shapes/halfspace.h>
#include <math/shapes/sphere.h>
#include <util/enum.h>

#include <ds/Flags.h>

namespace idk
{
	ENUM(FrustumSide, char,
		Left,
		Right,
		Up,
		Down,
		Far,
		Near
	)

	enum class FrustumFaceBits
	{
		eLeft =0b000001,
		eRight=0b000010,
		eUp   =0b000100,
		eDown =0b001000,
		eNear =0b010000,
		eFar = 0b100000,
	};
	template <> struct FlagTraits<FrustumFaceBits>
	{
		enum
		{
			allFlags = uint32_t(FrustumFaceBits::eLeft) | uint32_t(FrustumFaceBits::eRight) | uint32_t(FrustumFaceBits::eUp) | uint32_t(FrustumFaceBits::eDown) | uint32_t(FrustumFaceBits::eNear) | uint32_t(FrustumFaceBits::eFar) 
		};
	};


	using FrustumFaceFlags = Flags<FrustumFaceBits>;

	struct frustum
	{
		array<halfspace, FrustumSide::count> sides;

		frustum& operator*=(const mat4& tfm);
		frustum  operator*(const mat4& tfm) const;

		bool contains(const sphere& s) const;
		FrustumFaceFlags containment_test(const sphere& s) const;
	};

	struct cascadingFrustum {
		real cascadingEnd[3];
		aabb projBox[3];
	};


	array<vec4, 8> camera_vp_to_extremes(const mat4& vp_matrix);
	frustum camera_vp_to_frustum(const mat4& view_projection_matrix);
	aabb	camera_vp_to_bounding_box(const mat4& view_projection_matrix);
	cascadingFrustum camera_vp_to_tight_bound(const mat4& view_projection_matrix);

	/*vec3{-1, 1,-1},//*/
	/*vec3{ 1, 1,-1},//*/
	/*vec3{ 1,-1,-1},//*/
	/*vec3{-1,-1,-1},//*/
	/*vec3{-1, 1, 1},//*/
	/*vec3{ 1, 1, 1},//*/
	/*vec3{ 1,-1, 1},//*/
	/*vec3{-1,-1, 1},//*/
	array<vec3, 8> frustum_points(const frustum& frust);

}