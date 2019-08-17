#pragma once
#include <memory>
#include "math2.h"
class debug_info;
class Vulkan;

using GfxSystem = Vulkan;
enum DbgShape
{
	eziPreBegin,
	eCube,
	eSquare,
	eziCount
};


class DebugRenderer
{
public:
	void Init(GfxSystem& system);
	void DrawShape(DbgShape shape, vec3 pos, vec3 scale, quat rotation);

	void Render();
private:
	std::shared_ptr<debug_info> info;

	struct pimpl;
	
	std::unique_ptr<pimpl> impl;

	struct Pipeline;

	void RegisterShaders(GfxSystem& system);
	void RegisterShapes (GfxSystem& system);
	std::unique_ptr<Pipeline> CreateRenderPipeline(GfxSystem& system);
};