#pragma once
#include <type_traits>
//Do this in global namespace after everything is defined.
#define MARK_NON_COPY_CTORABLE(fully_qualified_type_name)                               \
namespace std																	        \
{																				        \
	template<>	struct is_copy_constructible<fully_qualified_type_name> : false_type{}; \
}