#pragma once
#include <memory>
#include <gfx/pipeline_config.h>
#include <gfx/uniform_info.h>
#include <gfx/debug_vtx_layout.h>
#include <core/ISystem.h>
namespace idk
{

enum DbgShape
{
	eziPreBegin,
	eCube,
	eSquare,
	eEqTriangle,
	eziCount
};
struct debug_info
{
	struct inst_data
	{
		vec4 col;
		mat4 transform;
	};
	idk::hash_table<DbgShape, idk::vector<inst_data>> render_info;
};

//To bypass the long-af DrawShape func with simple default values.
struct DebugObject
{
	DbgShape shape;
	vec3 pos{};
	vec3 scale{1,1,1};
	vec3 axis{1,0,0};
	idk::rad angle{};
	vec4 color{1,1,1,1};
	DebugObject(DbgShape s) :shape{ s } {}
	DebugObject& Pos     (const vec3& p          ) { pos   = p              ; return*this; }
	DebugObject& Scale   (const vec3& s          ) { scale = s              ; return*this; }
	DebugObject& Rotation(const vec3& ax, rad ang) { axis  = ax; angle = ang; return*this; }
	DebugObject& Color   (const vec4& c          ) { color = c              ; return*this; }
};

class DebugRenderer :public idk::ISystem
{
public:
	//DebugRenderer()
	void Init() override;
	void DrawShape(DbgShape shape, vec3 pos, vec3 scale, vec3 axis, idk::rad angle, vec4 color);
	void DrawShape(const DebugObject& obj);

	virtual void Render();
	~DebugRenderer()override;
protected:
	virtual void Init(const idk::pipeline_config& pipeline_config)=0;
	std::shared_ptr<debug_info> info;

};
}