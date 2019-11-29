#pragma once
#include <res/Resource.h>
#include <res/ResourceHandle.h>
#include <res/GenericResourceHandle.h>

namespace idk
{
    struct GenericResourceHandle;

	struct ResourceBundle
	{
		template<typename T>
		struct ResourceSpan;
		
		ResourceBundle() = default;

		template<typename Res> // conversion from single resource
		ResourceBundle(const RscHandle<Res>&);

		size_t Count() const;
		template<typename T> RscHandle<T>    Get() const;    // get a resource from the bundle
		template<typename T> ResourceSpan<T> GetAll() const; // get all resources of one type
		span<const GenericResourceHandle>    GetAll() const; // gets absolutely all resources

		template<typename T> void Add(RscHandle<T> handle); // will reshuffle vector and invalidate span, but you shouldn't be accessing vector directly anyway so this is ok

		bool operator!=(const ResourceBundle&) const;
		bool operator==(const ResourceBundle&) const;
	private:
		struct sub_array { char index = 0, count = 0; };

		vector<GenericResourceHandle> handles; // always sorted so that we can simply span
		array<sub_array, ResourceCount> subarrays;
	};

	template<typename T>
	struct ResourceBundle::ResourceSpan 
	{
		struct iterator;

		ResourceSpan(const GenericResourceHandle* btr, const GenericResourceHandle* etr);

		iterator begin() const;
		iterator end() const;
		size_t   size() const;
	private:
		span<const GenericResourceHandle> span_over;
	};

	template<typename T>
	struct ResourceBundle::ResourceSpan<T>::iterator
		: comparable<typename ResourceBundle::ResourceSpan<T>::iterator>
	{
		iterator(const GenericResourceHandle*);
		
		const RscHandle<T>& operator*() const;
		const RscHandle<T>* operator->() const;
		iterator& operator++();
		iterator  operator++(int);

		bool operator<(const iterator&) const;
	private:
		const GenericResourceHandle* itr;
	};
}
#include "ResourceBundle.inl"