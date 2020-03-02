#pragma once
#include <idk.h>
#include <ds/lazy_vector.h>
namespace idk::gfxdbg
{


	struct DbgLifetime
	{
		string rsc_name;
		size_t rsc_id;
		size_t start, end;
	};


	using FgRscLifetimes = lazy_vector<vector<DbgLifetime>>;

	static inline constexpr string_view kLifetimeName= "_rsc_lifetime";
}