#include "pch.h"
#include "draw_call.h"
namespace idk::vkn
{
	void draw_call::SetBinding(const uniform_binding& uniform)
	{
		uniforms.emplace_back(uniform);
	}
	void draw_call::SetBindings(const vector<uniform_binding>& uniform)
	{
		uniforms.insert(uniforms.end(),uniform.begin(),uniform.end());
	}
	void draw_call::SetBinding (uniform_binding&& uniform)
	{
		uniforms.emplace_back(std::move(uniform));
	}
	void draw_call::SetBindings(vector<uniform_binding>&& uniform)
	{
		if (uniforms.size())
			for (auto& u : uniform)
			{
				uniforms.emplace_back(std::move(u));
			}
		else
			uniforms = std::move(uniform);
	}
}
