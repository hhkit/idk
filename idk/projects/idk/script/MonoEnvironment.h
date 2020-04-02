#pragma once
#include <idk.h>
#include <map>
#include <mono/jit/jit.h>

#include <script/ManagedType.h>
#include <script/MonoBehaviorData.h>

namespace idk::mono
{
	// contains the environment in which a system lives
	class MonoEnvironment
	{
	public:
		MonoEnvironment() = default;
		MonoImage*  Image() const noexcept;
		MonoDomain* Domain() const noexcept;
		const ManagedType* Type(string_view) const;

		virtual void Init() {};
		void ScanTypes();

		virtual ~MonoEnvironment() = default;
	protected:
		MonoDomain*   _domain   {};
		MonoAssembly* _assembly {};

		std::map<string, ManagedType> _types;

		MonoEnvironment(MonoEnvironment&&) noexcept = delete;
		MonoEnvironment(const MonoEnvironment&) = delete;
		MonoEnvironment& operator=(MonoEnvironment&&) noexcept = delete;
		MonoEnvironment& operator=(const MonoEnvironment&) = delete;
	};
}