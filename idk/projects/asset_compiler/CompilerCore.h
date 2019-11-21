#pragma once
#include <idk.h>

namespace idk
{
	class IAssetCompiler;

	class CompilerCore
	{
	public:
		CompilerCore();

		void SetDestination(string_view dest);

		template<typename T, typename ... Args>
		void RegisterCompiler(string_view ext, Args&& ...);

		void Compile(string_view full_path);
	private:
		hash_table<string_view, unique_ptr<IAssetCompiler>> _loaders;
		string destination = "build";
	};

	template<typename T, typename ...Args>
	void CompilerCore::RegisterCompiler(string_view ext, Args&& ...args)
	{
		_loaders.emplace(ext, std::make_unique<T>(std::forward<Args>(args)...));
	}
}