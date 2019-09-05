#pragma once
#include <ds/span.h>
#include <ds/slow_tree.h>
#include <idk.h>
#include <core/ISystem.h>
#include <core/Core.h>

namespace idk
{
	class SceneGraphSystem
		: public ISystem
	{
	public:
		using SceneGraph = slow_tree<Handle<class Gameobject>>;

		void BuildSceneGraph(span<const GameObject> objs);

		SceneGraph* FetchSceneGraph(Handle<class GameObject>);
	private:
		hash_table<
			Handle<class GameObject>, 
			SceneGraph*
		> sg_lookup;

		slow_tree<Handle<class GameObject>> scene_graphs;

		void Init() override;
		void Shutdown() override;
	};

}
