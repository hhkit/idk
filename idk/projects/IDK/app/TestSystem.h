#pragma once
#include <core/ISystem.h>

namespace idk
{
	class TestSystem : public ISystem
	{
	public:
		void Init() override;
		void TestSpan(span<const class Transform>);
		void Shutdown() override;
	};
}