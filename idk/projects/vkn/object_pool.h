#pragma once
#include <idk.h>
namespace idk::vkn
{

template<typename T>
class object_pool
{
public:
	using handle_t = size_t;
	using container_t = vector<std::pair<handle_t, T>>;
	template<typename ITR>
	struct iterator_t : public comparable<iterator_t<ITR>>
	{
		handle_t handle()const { return itr->first; }
		T& operator*()const
		{
			return itr->second;
		}
		T* operator->()const
		{
			return &itr->second;
		}
		iterator_t operator++(int)
		{
			auto result = iterator_t{ *this };
			++itr;
			return result;
		}
		iterator_t& operator++()
		{
			++itr;
			return *this;
		}
		bool operator<(const iterator_t& rhs)const { return itr < rhs.itr; }
		iterator_t() = default;
		iterator_t(ITR i) :itr{ i } {};
	private:
		ITR itr;
	};
	using iterator = iterator_t<typename container_t::iterator>;
	using citerator = iterator_t<typename container_t::const_iterator>;
	using riterator = iterator_t<typename container_t::reverse_iterator>;

	void reserve(size_t num_elems)
	{
		objects.reserve(num_elems);
	}

	template<typename...Args>
	handle_t add(Args&& ... args)
	{
		handle_t next = alloc_next_handle(objects.size());
		objects.emplace_back(std::make_pair(next, T{ std::forward<Args>(args)... }));
		return next;
	}
	const T& get(handle_t handle)const
	{
		auto itr = alloced.find(handle);
		if (itr == alloced.end())
			throw std::runtime_error("Attempting to dereference a handle that wasn't allocated.");
		return objects[itr->second].second;
	}
	T& get(handle_t handle)
	{
		auto itr = alloced.find(handle);
		if (itr == alloced.end())
			throw std::runtime_error("Attempting to dereference a handle that wasn't allocated.");
		return objects[itr->second].second;
	}

	T& operator[](size_t handle)
	{
		return get(handle);
	}
	const T& operator[](size_t handle)const
	{
		return get(handle);
	}

	size_t size()const { return objects.size(); }

	void free(handle_t freeing)
	{
		auto& back = objects.back();
		auto free_itr = alloced.find(freeing);
		if (free_itr == alloced.end())
			throw std::runtime_error("Attempting to free a handle that wasn't allocated.");
		auto back_itr = alloced.find(back.first);
		std::swap(back_itr->second, free_itr->second);
		
		auto& obj_new_back = objects[back_itr->second];
		auto& obj_new_free = objects[free_itr->second];
		std::swap(obj_new_back, obj_new_free);
		freed.emplace(freeing);
		alloced.erase(free_itr);
		objects.pop_back();
	}

	iterator begin() { return iterator{ objects.begin() }; }
	iterator end() { return iterator{ objects.end() }; }
	citerator cbegin()const { return citerator{ objects.cbegin() }; }
	citerator cend()const { return citerator{ objects.cend() }; }
	riterator rbegin() { return riterator{ objects.cbegin() }; }
	riterator rend() { return riterator{ objects.cend() }; }
private:
	using index_t = size_t;
	handle_t alloc_next_handle(index_t index)
	{
		handle_t result = index;
		while (alloced.find(result) != alloced.end())
		{
			if (!freed.empty())
			{
				result = *freed.begin();
				freed.erase(freed.begin());
			}
			else
			{
				result = ++largest;
			}
		}
		largest = std::max(largest, result);
		alloced.emplace(result, index);
		return result;
	}
	hash_set<handle_t> freed;
	hash_table<handle_t, index_t> alloced;
	handle_t largest{};
	container_t objects;
};
}