#pragma once
namespace idk::vkn
{
	class IAsyncLoader
	{
	public:
		virtual void Update() = 0;//Called once per frame at a safe place to replace stuff(for synchronization)
		virtual size_t NumAdded()const = 0;
		virtual size_t NumProcessed()const = 0;
		virtual ~IAsyncLoader() = default;
	};
}