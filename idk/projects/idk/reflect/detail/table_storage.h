#pragma once

namespace idk::reflect::detail
{
	template<typename T>
	struct class_holder {};

	// replacement for property::table_storage
	template<typename ClassT, typename... Ts>
	struct table_storage;
}