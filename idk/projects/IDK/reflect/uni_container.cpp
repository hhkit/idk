#include "stdafx.h"
#include <reflect/reflect.h>

namespace idk::reflect
{
	uni_container::iterator::iterator(const iterator& other)
		: base{ other.base }, iter{ other.iter }
	{}

	uni_container::iterator::iterator(uni_container::base* base, dynamic iter)
		: iter{ std::move(iter) }, base{ base }
	{}

	uni_container::iterator& uni_container::iterator::operator=(const iterator& other)
	{
		base = other.base;
		iter = other.iter;
		return *this;
	}

	uni_container::iterator& uni_container::iterator::operator++() //prefix increment
	{
		base->iter_inc(*this);
		return *this;
	};

	bool uni_container::iterator::operator==(const iterator& other) const
	{
		return base->iter_cmp(*this, other);
	}

	bool uni_container::iterator::operator!=(const iterator& other) const
	{
		return !base->iter_cmp(*this, other);
	}

	dynamic uni_container::iterator::operator*() const
	{
		return std::move(base->iter_deref(*this));
	}



	size_t uni_container::size() const
	{
		return ptr_->size();
	}

	uni_container::iterator uni_container::begin() const
	{
		return ptr_->begin();
	}

	uni_container::iterator uni_container::end() const
	{
		return ptr_->end();
	}

	void uni_container::add(const dynamic& elem)
	{
		return ptr_->add(elem);
	}

	void uni_container::clear()
	{
		return ptr_->clear();
	}
}