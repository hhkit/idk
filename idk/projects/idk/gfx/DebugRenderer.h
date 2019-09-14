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
	};

	static inline const auto default_color = color{ 1,0,0 };
	
	// world space debug render
	void Draw(RscHandle<Mesh> mesh, const mat4& transform, const color& c = default_color, seconds duration = seconds{ 0 }, bool depth_test = true);
	void Draw(const aabb&   bounding_box                 , const color& c = default_color, seconds duration = seconds{ 0 }, bool depth_test = true);
	void Draw(const box&    oriented_box                 , const color& c = default_color, seconds duration = seconds{ 0 }, bool depth_test = true);
	void Draw(const ray&    ray                          , const color& c = default_color, seconds duration = seconds{ 0 }, bool depth_test = true);
	void Draw(const sphere& sphere                       , const color& c = default_color, seconds duration = seconds{ 0 }, bool depth_test = true);

	span<const DebugInfo> GetWorldDebugInfo() const;
protected:
	vector<DebugInfo> debug_info;
};
}