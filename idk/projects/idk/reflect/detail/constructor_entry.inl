#pragma once

#include <reflect/reflect.h>

namespace idk::reflect
{

	namespace detail
	{
		template<typename Obj, typename... Params>
		void constructor_entry<Obj, Params...>::try_construct(dynamic* obj) const
		{
			obj;
			if constexpr (sizeof...(Params) == 0)
			{
				new (obj) dynamic{ Obj() };
			}
		}

		template<typename Obj, typename... Params>
		void constructor_entry<Obj, Params...>::try_construct(dynamic* obj, dynamic& _0) const
		{
			obj; _0;
			if constexpr (sizeof...(Params) == 1)
			{
				assert(_0.valid());
				if (!_0.is<T<0>>()) return;
				new (obj) dynamic{ Obj(_0.get<T<0>>()) };
			}
		}

		template<typename Obj, typename... Params>
		void constructor_entry<Obj, Params...>::try_construct(dynamic* obj, dynamic& _0, dynamic& _1) const
		{
			obj; _0; _1;
			if constexpr (sizeof...(Params) == 2)
			{
				assert(_0.valid() && _1.valid());
				if (!_0.is<T<0>>() && !_1.is<T<1>>()) return;
				new (obj) dynamic{ Obj(_0.get<T<0>>(), _1.get<T<1>>()) };
			}
		}

		template<typename Obj, typename... Params>
		void constructor_entry<Obj, Params...>::try_construct(dynamic* obj, dynamic& _0, dynamic& _1, dynamic& _2) const
		{
			obj; _0; _1; _2;
			if constexpr (sizeof...(Params) == 3)
			{
				assert(_0.valid() && _1.valid() && _2.valid());
				if (!_0.is<T<0>>() && !_1.is<T<1>>() && !_2.is<T<2>>()) return;
				new (obj) dynamic{ Obj(_0.get<T<0>>(), _1.get<T<1>>(), _2.get<T<2>>()) };
			}
		}

		template<typename Obj, typename... Params>
		void constructor_entry<Obj, Params...>::try_construct(dynamic* obj, dynamic& _0, dynamic& _1, dynamic& _2, dynamic& _3) const
		{
			obj; _0; _1; _2; _3;
			if constexpr (sizeof...(Params) == 4)
			{
				assert(_0.valid() && _1.valid() && _2.valid() && _3.valid());
				if (!_0.is<T<0>>() && !_1.is<T<1>>() && !_2.is<T<2>>() && !_3.is<T<3>>()) return;
				new (obj) dynamic{ Obj(_0.get<T<0>>(), _1.get<T<1>>(), _2.get<T<2>>(), _3.get<T<3>>()) };
			}
		}

		template<typename Obj, typename... Params>
		void constructor_entry<Obj, Params...>::try_construct(dynamic* obj, dynamic& _0, dynamic& _1, dynamic& _2, dynamic& _3, dynamic& _4) const
		{
			obj; _0; _1; _2; _3; _4;
			if constexpr (sizeof...(Params) == 5)
			{
				assert(_0.valid() && _1.valid() && _2.valid() && _3.valid() && _4.valid());
				if (!_0.is<T<0>>() && !_1.is<T<1>>() && !_2.is<T<2>>() && !_3.is<T<3>>() && !_4.is<T<4>>()) return;
				new (obj) dynamic{ Obj(_0.get<T<0>>(), _1.get<T<1>>(), _2.get<T<2>>(), _3.get<T<3>>(), _4.get<T<4>>()) };
			}
		}

		template<typename Obj, typename... Params>
		void constructor_entry<Obj, Params...>::try_construct(dynamic* obj, dynamic& _0, dynamic& _1, dynamic& _2, dynamic& _3, dynamic& _4, dynamic& _5) const
		{
			obj; _0; _1; _2; _3; _4; _5;
			if constexpr (sizeof...(Params) == 6)
			{
				assert(_0.valid() && _1.valid() && _2.valid() && _3.valid() && _4.valid() && _5.valid());
				if (!_0.is<T<0>>() && !_1.is<T<1>>() && !_2.is<T<2>>() && !_3.is<T<3>>() && !_4.is<T<4>>() && !_5.is<T<5>>()) return;
				new (obj) dynamic{ Obj(_0.get<T<0>>(), _1.get<T<1>>(), _2.get<T<2>>(), _3.get<T<3>>(), _4.get<T<4>>(), _5.get<T<5>>()) };
			}
		}

	} // namespace detail

}