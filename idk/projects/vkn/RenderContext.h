#pragma once
#include "RenderTask.h"
#include <meta/stl_hack.h>
namespace idk::meta
{
	namespace detail
	{

	template<typename T, typename ...Args>
	struct first_is_same : std::false_type {};
	template<typename T, typename A1, typename ...Args>
	struct first_is_same<T,A1,Args...> : std::is_same<T, A1> {};

	}
	template<typename T, typename ...Args>
	static constexpr bool first_is_same = detail::first_is_same<T,Args...>::value;

}
namespace idk::vkn
{
	class FrameGraphResourceManager;
	namespace FrameGraphDetail
	{
		struct Context : RenderTask
		{
			//TODO put the stuff that needs to be read from here.
			const FrameGraphResourceManager& Resources()const noexcept{ return *resources; }
			//FrameGraphResourceManager& Resources() noexcept { return *resources; }
			void SetRscManager(const FrameGraphResourceManager& mgr)
			{
				resources = &mgr;
			}
			template<typename ...Args,
				typename = std::enable_if_t<
				!idk::meta::first_is_same<Context,std::decay_t<Args>...>
				>
			>
			Context(Args&&... args) : RenderTask{std::forward<Args>(args)...}{}
			Context(const Context&) = default;
			Context(Context&&) = default;
			Context& operator=(const Context&) = default;
			Context& operator=(Context&&) = default;
		private:
			const FrameGraphResourceManager* resources;
		};

		using Context_t = Context&;
	}
}
MARK_NON_COPY_CTORABLE(idk::vkn::FrameGraphDetail::Context);