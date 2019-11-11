#pragma once
#include <memory>
#include <math/shapes.h>
#include <core/ISystem.h>
namespace idk
{
class DebugRenderer 
	: public ISystem
{
public:
	struct DebugInfo
	{
		RscHandle<Mesh> mesh;
		mat4            transform;
		color           color;
		seconds         duration;
		bool            depth_test;

		seconds         display_time;
	};

	static inline const auto default_color = color{ 1,0,0 };
	
	// world space debug render
	void Draw(RscHandle<Mesh> mesh, const mat4& transform  , const color& c = default_color, seconds duration = seconds{ 0 }, bool depth_test = true);
	void Draw(const aabb&     bounding_box                 , const color& c = default_color, seconds duration = seconds{ 0 }, bool depth_test = true);
	void Draw(const box&      oriented_box                 , const color& c = default_color, seconds duration = seconds{ 0 }, bool depth_test = true);
	void Draw(const capsule&  capsule                      , const color& c = default_color, seconds duration = seconds{ 0 }, bool depth_test = true);
	void Draw(const ray&      ray                          , const color& c = default_color, seconds duration = seconds{ 0 }, bool depth_test = true);
	void Draw(const vec3&	  start, const vec3& end	   , const color& c = default_color, seconds duration = seconds{ 0 }, bool depth_test = true); //Same as ray, but without the tetrahedron(Arrow pointer)
	void Draw(const sphere&   sphere                       , const color& c = default_color, seconds duration = seconds{ 0 }, bool depth_test = true);

	void GraphicsTick();
	span<const DebugInfo> GetWorldDebugInfo() const;
private:
	void Init() override {};
	void Shutdown() override {};
	vector<DebugInfo> debug_info;
};
}