#pragma once
#include <utility>
namespace idk
{
	template<typename T>
	struct comparable
	{
		bool operator>(const T&)  const;
		bool operator<=(const T&) const;
		bool operator>=(const T&) const;
		bool operator==(const T&) const;
		bool operator!=(const T&) const;
	private:
		T& me();
		const T& me() const;
	};
}
