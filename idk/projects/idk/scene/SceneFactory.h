#pragma once
#include <scene/Scene.h>
#include <res/ResourceFactory.h>

namespace idk
{
	class SceneFactory
		: public ResourceFactory<Scene>
	{
	public:
		unique_ptr<Scene> GenerateDefaultResource() override;
		unique_ptr<Scene> Create() noexcept override;
		unique_ptr<Scene> Create(PathHandle h) noexcept override;
	};
}