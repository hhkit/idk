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
            _scene_graph->_per_scene[_root.scene].children.size() > _root.index;
	}

    size_t SceneGraphHandle::GetNumChildren() const
    {
        return _scene_graph->_per_scene[_root.scene].children[_root.index].size();
    }

    Handle<GameObject> SceneGraphHandle::GetGameObject() const
    {
        return _root;
    }

	SceneGraphHandle::iterator SceneGraphHandle::begin() const
	{
        return iterator{ *this, _scene_graph->_per_scene[_root.scene].children[_root.index].begin() };
	}

    SceneGraphHandle::iterator SceneGraphHandle::end() const
    {
        return iterator{ *this, _scene_graph->_per_scene[_root.scene].children[_root.index].end() };
    }



    SceneGraphHandle::iterator::iterator(const SceneGraphHandle& handle, const GenericHandle::index_t* ptr)
        : _scene_graph{ handle._scene_graph }, _root{ handle._root }, ptr{ ptr }
    {
    }

    Handle<GameObject> SceneGraphHandle::iterator::operator*() const
    {
        return Handle<GameObject>{ *ptr, _scene_graph->_per_scene[_root.scene].gens[*ptr], _root.scene };
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
            ps.children.clear();
            ps.gens.clear();
            ps.root.clear();
        }

        for (const auto& o : objs)
        {
            Insert(o.GetHandle());
        }
    }

    void SceneGraph::Insert(Handle<GameObject> handle)
    {
        if (static_cast<size_t>(handle.index) >= _per_scene[handle.scene].children.size())
        {
            _per_scene[handle.scene].children.resize(handle.index + 1);
            _per_scene[handle.scene].gens.resize(handle.index + 1);
        }
        _per_scene[handle.scene].gens[handle.index] = handle.gen;

        if (const auto parent = handle->Parent())
            _per_scene[handle.scene].children[parent.index] += handle.index;
        else
            _per_scene[handle.scene].root += handle.index;
    }

    void SceneGraph::Reparent(Handle<GameObject> handle, Handle<GameObject> old_parent)
    {
        auto& children = _per_scene[old_parent.scene].children[old_parent.index];
        const auto pos = children.find(handle.index);
        if (pos != string::npos)
            children.erase(pos, 1);
        Insert(handle);
    }

    SceneGraphHandle SceneGraph::GetHandle(Handle<GameObject> root) const
    {
        return SceneGraphHandle{ this, root };
    }
}
