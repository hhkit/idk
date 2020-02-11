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

	SceneGraphHandle::iterator SceneGraphHandle::begin() const
	{
        return iterator{ *this, _scene_graph->_per_scene[_root.scene].nodes[_root.index].begin() };
	}

    SceneGraphHandle::iterator SceneGraphHandle::end() const
    {
        return iterator{ *this, _scene_graph->_per_scene[_root.scene].nodes[_root.index].end() };
    }



    SceneGraphHandle::iterator::iterator(const SceneGraphHandle& handle, const GenericHandle::index_t* ptr)
        : _scene_graph{ handle._scene_graph }, _root{ handle._root }, ptr{ ptr }
    {
    }

    Handle<GameObject> SceneGraphHandle::iterator::operator*() const
    {
        return Handle<GameObject>{ *ptr, _root.gen, _root.scene };
    }

    SceneGraphHandle::iterator& SceneGraphHandle::iterator::operator++()
    {
        ++ptr;
        return *this;
    }

    SceneGraphHandle::iterator SceneGraphHandle::iterator::operator++(int)
    {
        auto copy = *this;
        ++ptr;
        return copy;
    }

    bool SceneGraphHandle::iterator::operator==(const SceneGraphHandle::iterator& other) const
    {
        return ptr == other.ptr;
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
