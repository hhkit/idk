#pragma once
#include <utility>
#include <vector>
#include <mutex>

//#include <parallel/queue.h>
//#include <parallel/ThreadPool.h>

namespace idk::meta
{
	template<typename T, typename ...CtorArgs>
	struct is_copy_ctor : std::false_type {};

	template<typename T, typename T1>
	struct is_copy_ctor<T, T1> : std::conditional_t<std::is_same_v<T1, const T&>, std::true_type, std::false_type> {};
	template<typename T, typename ...CtorArgs>
	constexpr bool is_copy_ctor_v = is_copy_ctor<T, CtorArgs...>::value;


	template<typename T, typename ...CtorArgs>
	struct is_move_ctor : std::false_type {};

	template<typename T, typename T1>
	struct is_move_ctor<T, T1> : std::conditional_t<std::is_same_v<T1, T&&>, std::true_type, std::false_type> {};
	template<typename T, typename ...CtorArgs>
	static constexpr bool is_move_ctor_v = is_move_ctor<T, CtorArgs...>::value;

	template<typename T, typename ...CtorArgs>
	struct is_def_ctor : std::false_type {};

	template<typename T>
	struct is_def_ctor<T> : std::true_type {};
	template<typename T, typename ...CtorArgs>
	static constexpr bool is_def_ctor_v = is_def_ctor<T, CtorArgs...>::value;

	template<typename T, typename ...CtorArgs>
	using disable_if_std_ctor = std::enable_if_t < !(is_copy_ctor_v<T, CtorArgs...> || is_move_ctor_v<T,CtorArgs...> || is_def_ctor_v<T,CtorArgs...>)>;

}


namespace idk
{
	void DoNothing2(bool = true);
	struct PCTracker
	{
		static bool should_block;
		static void Deregister(const void* pool,size_t sz);
		static void Register(const void* pool,size_t sz);
		static std::atomic<size_t> size;
		static size_t TotalSize();
	};

	template<typename T, typename = void>
	struct HasEmpty : std::false_type
	{
	};
	template<typename T>
	struct HasEmpty<T, decltype(static_cast<void>(std::declval<const T&>().empty()))> : std::true_type
	{
	};
	template<typename T, typename = void>
	struct HasCapacity : std::false_type
	{
	};
	template<typename T>
	struct HasCapacity<T, decltype(static_cast<void>(std::declval<const T&>().capacity()))> : std::true_type
	{
	};

	//container_t needs to use allocation history (i.e. a capacity)
	template<typename base>
	struct PooledContainer : base
	{
		template<typename T>
		using container_t = std::vector<T>;
		static size_t Num()
		{
			return GetContainerPool().size();
		}
		PooledContainer() :base{ GetContainer() }
		{

		}
		template<typename T>
		PooledContainer(std::initializer_list<T>&& il) : PooledContainer{}
		{
			base& me = *this;
			me = std::move(il);

		}
		//template<typename Arg,typename = meta::disable_if_std_ctor<PooledContainer,Arg&&...>>
		//PooledContainer(Arg&& arg) : base{ GetContainer() }
		//{
		//	base& me = *this;
		//	me = std::forward<Arg>(arg);
		//}
		template<typename ...Args>
		PooledContainer(size_t n, Args&& ... args) : base{ Resize(GetContainer(),n,std::forward<Args>(args)...) } {}
		PooledContainer(const PooledContainer& v) : base{ GetContainer() }
		{
			(*this) = v;
		}
		PooledContainer(PooledContainer&&)noexcept(noexcept(base{ std::declval<base>() })) = default;
		PooledContainer& operator=(const PooledContainer<base>& v) = default;
		PooledContainer& operator=(PooledContainer<base>&&)noexcept(noexcept(std::declval<base&>() = std::move(std::declval<base>()))) = default;
		//PooledContainer& operator=(const base& rhs)
		//{
		//	base::operator=(rhs);
		//	return *this;
		//}
		//PooledContainer& operator=(base&& rhs) noexcept(noexcept(std::declval<base&>() = std::declval<base>()))
		//{
		//	base::operator=(std::move(rhs));
		//	return *this;
		//}
		using base::operator=;

		~PooledContainer()
		{
			this->clear();
			if constexpr (HasCapacity<base>::value)
			{
				if (this->capacity() == 0)
				{
					DoNothing2(false);
					return;
				}
			}
			AddContainer(*this);
		}

	private:
		static std::mutex m;
		static std::mutex& get_mutex()
		{
			return  m;
		}

		static container_t<base> container_pool_;
		static container_t<base>& GetContainerPool()
		{
			return container_pool_;
		}
		template<typename ...Args>
		static base&& Resize(base&& v, size_t n, Args&& ... args)
		{
			v.resize(n, std::forward<Args>(args)...);
			return std::move(v);
		}
		static base GetContainer()
		{
			auto& pool = GetContainerPool();
			//auto obj =pool.pop_front(mt::thread_id());
			////PCTracker::Deregister(&pool, 1);// pool.size());
			//if (!obj)
			//{
			//	DoNothing2();
			//	return base{};
			//}
			//return  std::move(*obj);

			std::lock_guard guard{ get_mutex() };
			if (pool.empty())
			{
				DoNothing2();
				return base{};

			}
			auto result = std::move(pool.back());
			pool.pop_back();
			return result;
		}
		static void AddContainer(base& v)
		{
			auto& pool = GetContainerPool();
			//pool.emplace_back(mt::thread_id(),std::move(v));
			
			std::lock_guard guard{ get_mutex() };
			pool.emplace_back(std::move(v));
		}
	};
	template<typename base>
	PooledContainer<base>::container_t<base> PooledContainer<base>::container_pool_{};
	template<typename base>
	std::mutex PooledContainer<base>::m{};

}