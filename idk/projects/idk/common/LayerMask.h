#pragma once
#include <cstdint>

namespace idk
{
	// On Compiler Explorer, this seems to be optimized away with /O2.
	struct LayerMask
	{
        std::uint32_t mask = 0;

		LayerMask() noexcept = default;
		explicit LayerMask(int32_t m) noexcept : mask{ reinterpret_cast<uint32_t&>(m) } {}
		explicit LayerMask(uint32_t m) noexcept : mask{ m } {}

		operator bool() const noexcept { return mask; }
		bool operator!() const noexcept { return !mask; }
		inline LayerMask operator~() const noexcept{ return LayerMask{ ~mask };}
		inline LayerMask& operator|=(LayerMask rhs) noexcept { return *this = LayerMask{ mask | rhs.mask }; }
		inline LayerMask& operator&=(LayerMask rhs) noexcept { return *this = LayerMask{ mask & rhs.mask }; }
		inline LayerMask& operator^=(LayerMask rhs) noexcept { return *this = LayerMask{ mask ^ rhs.mask }; }
		inline LayerMask Not()                const noexcept { return operator~();}
		inline LayerMask And(LayerMask other) const noexcept { return LayerMask{ mask & other.mask}; }
		inline LayerMask Or (LayerMask other) const noexcept { return LayerMask{ mask | other.mask}; }
		inline LayerMask Xor(LayerMask other) const noexcept { return LayerMask{ mask ^ other.mask}; }
	};

	inline LayerMask operator|(LayerMask lhs, LayerMask rhs) noexcept { return lhs.Or (rhs); }
	inline LayerMask operator&(LayerMask lhs, LayerMask rhs) noexcept { return lhs.And(rhs); }
	inline LayerMask operator^(LayerMask lhs, LayerMask rhs) noexcept { return lhs.Xor(rhs); }
}
