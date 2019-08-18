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

class DebugRenderer :public idk::ISystem
{
public:
	//DebugRenderer()
	void Init() override;
	void DrawShape(DbgShape shape, vec3 pos, vec3 scale, vec3 axis, idk::rad angle, vec4 color);

	void Shutdown() override {}
	virtual void Render();
	~DebugRenderer()override;
protected:
	virtual void Init(const idk::pipeline_config& pipeline_config, const idk::uniform_info& uniform_info)=0;
	std::shared_ptr<debug_info> info;

};
}