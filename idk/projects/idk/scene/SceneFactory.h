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
		unique_ptr<Scene> Create() override;
	};

	class SceneLoader
		: public IFileLoader
	{
		ResourceBundle LoadFile(PathHandle filepath) override;
	};
}