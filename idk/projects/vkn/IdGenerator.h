#pragma once

namespace idk::vkn::hlp
{
	template<typename id_t,bool reserve_zero=true>
	struct IdGenerator
	{
		id_t gen_next()
		{
			return _next_id++;
		}
		void reset_ids()
		{
			_next_id = _def;
		}
	private:
		static constexpr id_t _def = (reserve_zero) ? id_t{ 1 } : id_t{ 0 };
		id_t _next_id = _def;
	};
}