#pragma once
#include <map>
#include <ds/dual_set.inl>
#include <script/MonoEnvironment.h>

namespace idk
{
	using RpcId = int;
}

namespace idk::mono
{
	class MonoBehaviorEnvironment
		: public MonoEnvironment
	{
	public:
		MonoBehaviorEnvironment(string_view full_path_to_game_dll);
		~MonoBehaviorEnvironment();
		void Init() override;
		void Execute();
		ManagedType* GetBehaviorMetadata(string_view name);
		span<const char* const> GetBehaviorList();

		RpcId GetRpcIdFor(string_view function_name) const;
		string_view GetFunctionNameFromRpcId(RpcId) const;

	private:
		string assembly_data;
		vector<const char*> name_list;
		RpcId rpc_count = {};
		dual_set<RpcId, string> rpcmap;
		std::map<string, ManagedType*> mono_behaviors;
		void FindMonoBehaviors();
	};
}