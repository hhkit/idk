#pragma once
#include <core/GameObject.h>
#include <future>
namespace idk
{
	struct ColorPickResult
	{
		using result_t = Handle<GameObject>;
		bool ready(){ 
			return stored_result || (result.valid() && result.wait_for(std::chrono::seconds{ 0 })==std::future_status::ready);
		}
		result_t get()
		{ 
			return (stored_result)?*stored_result: *(stored_result = result.get());
		}
		ColorPickResult(std::promise<result_t>& promise) : result{ promise.get_future() } {}
	private:
		std::future<result_t> result;
		std::optional<result_t> stored_result;
	};
}