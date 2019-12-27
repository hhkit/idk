#pragma once
#include <scene/Scene.h>
#include <res/ResourceFactory.h>
#include <res/FileLoader.h>

namespace idk
{
	class SceneFactory
		: public ResourceFactory<Scene>
	{
	public:
		unique_ptr<Scene> GenerateDefaultResource() override;
		unique_ptr<Scene> Create() noexcept override;
	};

	class SceneLoader
		: public IFileLoader
	{
		ResourceBundle LoadFile(PathHandle filepath, const MetaBundle& bundle) override;
	};
}