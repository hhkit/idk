#pragma once

namespace idk::reflect
{
	class dynamic;

	namespace detail
	{
		struct constructor_entry_base
		{
			virtual void try_construct(dynamic* obj) const = 0;
			virtual void try_construct(dynamic* obj, dynamic& _0) const = 0;
			virtual void try_construct(dynamic* obj, dynamic& _0, dynamic& _1) const = 0;
			virtual void try_construct(dynamic* obj, dynamic& _0, dynamic& _1, dynamic& _2) const = 0;
			virtual void try_construct(dynamic* obj, dynamic& _0, dynamic& _1, dynamic& _2, dynamic& _3) const = 0;
			virtual void try_construct(dynamic* obj, dynamic& _0, dynamic& _1, dynamic& _2, dynamic& _3, dynamic& _4) const = 0;
			virtual void try_construct(dynamic* obj, dynamic& _0, dynamic& _1, dynamic& _2, dynamic& _3, dynamic& _4, dynamic& _5) const = 0;

			virtual ~constructor_entry_base() = default;
		};

		template<typename Obj, typename... Params>
		struct constructor_entry : constructor_entry_base
		{
			template<unsigned N> using T = std::tuple_element_t<N, std::tuple<Params...>>;

			void try_construct(dynamic* obj) const override;
			void try_construct(dynamic* obj, dynamic& _0) const override;
			void try_construct(dynamic* obj, dynamic& _0, dynamic& _1) const override;
			void try_construct(dynamic* obj, dynamic& _0, dynamic& _1, dynamic& _2) const override;
			void try_construct(dynamic* obj, dynamic& _0, dynamic& _1, dynamic& _2, dynamic& _3) const override;
			void try_construct(dynamic* obj, dynamic& _0, dynamic& _1, dynamic& _2, dynamic& _3, dynamic& _4) const override;
			void try_construct(dynamic* obj, dynamic& _0, dynamic& _1, dynamic& _2, dynamic& _3, dynamic& _4, dynamic& _5) const override;
		};
	}

}