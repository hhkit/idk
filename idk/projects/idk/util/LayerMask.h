#pragma once
#include <cstdint>
#include <compare>
namespace idk
{

	//On Compiler Explorer, this seems to be optimized away with /O2.
	struct LayerMask
	{
		std::int32_t mask{};
		LayerMask()noexcept = default;
		explicit LayerMask(int32_t m)noexcept : mask{ m } {}
		explicit LayerMask(uint32_t m)noexcept : mask{static_cast<int32_t>(m)}{}
		//LayerMask(int m)noexcept : mask{static_cast<int32_t>(m)}{}
		auto operator<=> (LayerMask rhs) const noexcept { return mask <=> rhs.mask; }
		operator bool()const noexcept { return mask; }
		bool operator!()const noexcept { return !mask; }
		inline LayerMask operator~()const noexcept{return LayerMask{ ~mask };}
		inline LayerMask& operator|=(LayerMask rhs)noexcept{return *this = LayerMask{ mask | rhs.mask };}
		inline LayerMask& operator&=(LayerMask rhs)noexcept{return *this = LayerMask{ mask & rhs.mask };}
		inline LayerMask& operator^=(LayerMask rhs)noexcept{return *this = LayerMask{ mask ^ rhs.mask };}
		inline LayerMask Not()               const noexcept{ return operator~();}
		inline LayerMask And(LayerMask other)const noexcept{ return LayerMask{ mask & other.mask};}
		inline LayerMask Or (LayerMask other)const noexcept{ return LayerMask{ mask | other.mask};}
		inline LayerMask Xor(LayerMask other)const noexcept{ return LayerMask{ mask ^ other.mask};}
	};
	inline LayerMask operator|(LayerMask lhs, LayerMask rhs)noexcept{return lhs.Or (rhs);}
	inline LayerMask operator&(LayerMask lhs, LayerMask rhs)noexcept{return lhs.And(rhs);}
	inline LayerMask operator^(LayerMask lhs, LayerMask rhs)noexcept{return lhs.Xor(rhs);}
}
