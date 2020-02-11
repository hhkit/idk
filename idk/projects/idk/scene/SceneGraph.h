#pragma once

#include <idk.h>
#include <core/GameObject.h>

namespace idk
{
	class SceneGraph;
	class SceneGraphHandle;

	class SceneGraphHandle
	{
	public:
		SceneGraphHandle(const SceneGraph* scene_graph, Handle<GameObject> root);
		template<typename Visitor> void Visit(Visitor&& visitor) const;

		explicit operator bool() const;
		size_t GetNumChildren() const;
		Handle<GameObject> GetGameObject() const;

		class iterator
		{
		public:
			iterator(const SceneGraphHandle& handle, const GenericHandle::index_t* ptr);
			~iterator() = default;
			iterator(const iterator&) = default;
			iterator& operator=(const iterator&) = default;

			Handle<GameObject> operator*() const;
			iterator& operator++(); //prefix increment
			iterator operator++(int); //postfix increment
			bool operator==(const iterator&) const;

		private:
			const SceneGraph* _scene_graph;
			Handle<GameObject> _root;
			const GenericHandle::index_t* ptr;
		};

		iterator begin() const;
		iterator end() const;

	private:
		const SceneGraph* _scene_graph;
		Handle<GameObject> _root;
	};

	class SceneGraph
	{
	public:
		void Build(span<const GameObject> objs);
		SceneGraphHandle GetHandle(Handle<GameObject> root = {});
		template<typename Visitor> void Visit(Visitor&& visitor, Handle<GameObject> root = {}) const;

	private:
		struct FlatHierarchy
		{
			vector<small_string<GenericHandle::index_t>> nodes;
			vector<GenericHandle::gen_t> gens;
		} _per_scene[MaxScene];

		friend SceneGraphHandle;

		template<typename Visitor> void VisitImpl(Visitor&& visitor, Handle<GameObject> root, int& depth, int& last_depth) const;
	};
}