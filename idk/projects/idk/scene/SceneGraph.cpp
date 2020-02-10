#include "stdafx.h"
#include "SceneGraph.h"

namespace idk
{

    SceneGraphHandle::SceneGraphHandle(const SceneGraph* scene_graph, Handle<GameObject> root)
        : _scene_graph{ scene_graph }, _root{ root }
    {
    }

	SceneGraphHandle::operator bool() const
	{
        return _scene_graph && 
            (_root || _root == Handle<GameObject>{}) &&
            _scene_graph->_per_scene[_root.scene].nodes.size() > _root.index;
	}

    size_t SceneGraphHandle::GetNumChildren() const
    {
        return _scene_graph->_per_scene[_root.scene].nodes[_root.index].size();
    }

    Handle<GameObject> SceneGraphHandle::GetGameObject() const
    {
        return _root;
    }

	void SceneGraph::Build(span<const GameObject> objs)
	{
        for (auto& ps : _per_scene)
        {
            ps.nodes.clear();
            ps.gens.clear();
        }

        for (const auto& o : objs)
        {
            const auto handle = o.GetHandle();

            if (static_cast<size_t>(handle.index) >= _per_scene[handle.scene].nodes.size())
            {
                _per_scene[handle.scene].nodes.resize(handle.index + 1);
                _per_scene[handle.scene].gens.resize(handle.index + 1);
            }
            _per_scene[handle.scene].gens[handle.index] = handle.gen;
            if (const auto parent = o.Parent())
                _per_scene[handle.scene].nodes[parent.index] += handle.index;
        }
	}

    SceneGraphHandle SceneGraph::GetHandle(Handle<GameObject> root)
    {
        return SceneGraphHandle{ this, root };
    }

}
