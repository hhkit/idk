#pragma once

namespace idk::reflect
{
	class type;

	namespace detail
	{
		template<typename Visitor>
		void visit(void* obj, type type, Visitor&& visitor, int& depth, int& last_visit_depth);

		template<typename K, typename V, typename Visitor>
		void visit_key_value(K&& key, V&& val, Visitor&& visitor, int& depth, int& curr_depth);
	}
}