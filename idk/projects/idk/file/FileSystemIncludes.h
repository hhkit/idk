#pragma once

namespace idk
{
	enum class FS_ERROR_CODE
	{
		FILESYSTEM_OK,
		FILESYSTEM_OTHER,
		FILESYSTEM_NO_MEM,
		FILESYSTEM_NO_INIT,
		FILESYSTEM_IS_INIT,
		FILESYSTEM_PAST_EOF,
		FILESYSTEM_NOT_MOUNTED,
		FILESYSTEM_NOT_FOUND,
		FILESYSTEM_NOT_FILE,
		FILESYSTEM_ONLY_READ,
		FILESYSTEM_ONLY_WRITE,
		FILESYSTEM_NOT_OPEN,
		FILESYSTEM_DUPLICATE,
		FILESYSTEM_FILE_CHANGED,
		FILESYSTEM_BAD_FILENAME,
		FILESYSTEM_BAD_ARGUMENT
	};

	enum class FS_QUERY_TYPE
	{
		EXT		= 1 << 0,
		CHANGE	= 1 << 1,
		ALL		= 1 << 2
	};

	enum class FS_CHANGE_STATUS
	{
		NO_CHANGE,
		CREATED,
		DELETED,
		RENAMED,
		WRITTEN,
		PATH_CHANGED,

		INVALID = -1
	};

	enum class FS_PERMISSIONS
	{
		READ,
		WRITE,
		APPEND,

		NONE
	};

	enum class FS_FILTERS : int
	{
		NONE			= 1 << 0,
		FILE			= 1 << 1,
		DIR				= 1 << 2,
		EXT				= 1 << 3,
		RECURSE_DIRS	= 1 << 4,

		ALL				= FILE | DIR | RECURSE_DIRS,
		ALL_NO_RECURSE	= FILE | DIR
	};

	template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
	inline auto operator|(T lhs, T rhs) noexcept
	{
		using UType = std::underlying_type_t<T>;
		return s_cast<T>(s_cast<UType>(lhs) | s_cast<UType>(rhs));
	}

	template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
	inline auto operator&(T lhs, T rhs)
	{
		using UType = std::underlying_type_t<T>;
		return s_cast<T>(s_cast<UType>(lhs) & s_cast<UType>(rhs));
	}

	template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
	inline auto operator^(T lhs, T rhs)
	{
		using UType = std::underlying_type_t<T>;
		return s_cast<T>(s_cast<UType>(lhs) ^ s_cast<UType>(rhs));
	}

	template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
	inline auto& operator|=(T& lhs, T rhs)
	{
		using UType = std::underlying_type_t<T>;
		lhs = s_cast<T>(s_cast<UType>(lhs) | s_cast<UType>(rhs));
		return lhs;
	}

	template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
	inline auto& operator&=(T & lhs, T rhs)
	{
		using UType = std::underlying_type_t<T>;
		lhs = s_cast<T>(s_cast<UType>(lhs) & s_cast<UType>(rhs));
		return *lhs;
	}

	template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
	inline auto& operator^=(T & lhs, T rhs)
	{
		using UType = std::underlying_type_t<T>;
		lhs = s_cast<T>(s_cast<UType>(lhs) ^ s_cast<UType>(rhs));
		return lhs;
	}

	template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
	bool enum_eq(T lhs, T rhs)
	{
		using UType = std::underlying_type_t<T>;
		return (lhs & rhs) == rhs;
	}
}