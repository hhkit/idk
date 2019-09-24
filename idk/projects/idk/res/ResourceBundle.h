#pragma once
#include <res/Resource.h>
#include <res/ResourceHandle.h>
#include <res/GenericResourceHandle.h>

namespace idk
{
	struct ResourceBundle
	{
		template<typename T>
		struct ResourceSpan;
		
		template<typename T> RscHandle<T>    Get() const;    // get a resource from the bundle
		template<typename T> ResourceSpan<T> GetAll() const; // get all resources of one type
		span<const GenericResourceHandle>    GetAll() const; // gets absolutely all resources

		template<typename T> void Add(RscHandle<T> handle) const; // will reshuffle vector and invalidate span, but you shouldn't be accessing vector directly anyway so this is ok
	private:
		struct sub_array { char index = 0, count = 0; };

		vector<GenericResourceHandle> handles; // always sorted so that we can simply span
		array<sub_array, ResourceCount> subarrays;
	};

	template<typename T>
	struct ResourceBundle::ResourceSpan 
	{
		struct iterator;

		ResourceSpan(GenericResourceHandle* btr, GenericResourceHandle* etr);

		iterator begin() const;
		iterator end() const;
		size_t   size() const;
	private:
		span<GenericResourceHandle> span_over;
	};

	template<typename T>
	struct ResourceBundle::ResourceSpan<T>::iterator
		: comparable<typename ResourceBundle::ResourceSpan<T>::iterator>
	{
		iterator(GenericResourceHandle*);

		T& operator*() const;
		T* operator->() const;
		iterator& operator++();
		iterator  operator++(int);

		bool operator<(const iterator&) const;
	private:
		GenericResourceHandle* itr;
	};
}
#include "ResourceBundle.inl"