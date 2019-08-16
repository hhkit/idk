#pragma once
#include <scene/Scene.h>
#include <res/ResourceFactory.h>

namespace idk
{
	class SceneFactory
		: public ResourceFactory<Scene>
	{
	public:
		unique_ptr<Scene> Create();
		unique_ptr<Scene> Create(string_view filepath, const ResourceMeta&);
	private:
	};
}