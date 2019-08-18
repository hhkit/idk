#pragma once
#include <memory>
#include "math2.h"
#include <gfx/pipeline_config.h>
#include <gfx/uniform_info.h>
#include <gfx/debug_vtx_layout.h>
class Vulkan;

using GfxSystem = Vulkan;
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


class DebugRenderer
{
public:
	void Init(GfxSystem& system);
	void DrawShape(DbgShape shape, vec3 pos, vec3 scale, quat rotation);

	virtual void Render();
protected:
	virtual void Init(GfxSystem& system,const idk::pipeline_config& pipeline_config, const idk::uniform_info& uniform_info)=0;
	std::shared_ptr<debug_info> info;
	struct pimpl;
	
	std::unique_ptr<pimpl> impl;

	struct Pipeline;

	void RegisterShaders(GfxSystem& system);
	void RegisterShapes (GfxSystem& system);
	std::unique_ptr<Pipeline> CreateRenderPipeline(GfxSystem& system);
};