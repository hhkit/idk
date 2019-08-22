#pragma once


namespace idk
{
	class ISystem
	{
	public:
		virtual void Init() = 0;
		virtual void LateInit() {};
		virtual void Shutdown() = 0;
		virtual ~ISystem() = default;
	};
}