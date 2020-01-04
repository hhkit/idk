#pragma once
#include <idk.h>
namespace idk
{

	template<typename Node, typename Key, typename Policy>
	struct TraversePolicy
	{
		using node_t = Node;
		using key_t = Key;
		TraversePolicy(Policy& pol) :policy{ pol } {}

		//Returns true to continue getting deeper edges, or false to not explore the node any further.
		bool Visit(const Node& node)
		{
			return policy.Policy::Visit(node);
		}
		//Returns a range of edges that lead deeper into the graph. Edge is used by GetNodeInfo, and hence must be compatible.
		auto DeeperEdges(const Node& node)
		{
			return policy.Policy::DeeperEdges(node);
		}
		//Returns an info_t object by copy, that can be passed int GetKeyFromInfo and GetNodeFromInfo which can return the Key and Node by reference respectively.
		template<typename Edge>
		auto GetNodeInfo(const Node& node, const Edge& edge)
		{
			return policy.Policy::GetNodeInfo(node, edge);
		}
		//Extracts the Key from the info
		template<typename Info>
		const Key& GetKeyFromInfo(const Info& info)
		{
			return policy.Policy::GetKeyFromInfo(info);
		}
		//Extracts the Node from the info
		template<typename Info>
		const Node& GetNodeFromInfo(const Info& info)
		{
			return policy.Policy::GetNodeFromInfo(info);
		}
		Policy& policy;
	};
	template<typename Node>
	using DepthFirst = std::stack<Node, vector<Node>>;
	template<typename Node>
	using BreadthFirst = std::queue<Node, vector<Node>>;

	template<typename Node, typename Key = Node, template <typename Node_t> typename TraverseType = DepthFirst>
	struct GraphTraverser
	{
		template<typename Policy>
		using VisitPolicy = TraversePolicy<Node, Key, Policy>;
		void Push(Node node)
		{
			frontier.push(node);
		}
		std::optional<Node> Pop()
		{
			if (!frontier.empty())
				return {};
			auto result = frontier.top();
			frontier.pop();
			return result;
		}
		template<typename Policy>
		bool VisitNextNode(Policy& policy_detail)
		{
			VisitPolicy<Policy> policy{ policy_detail };
			auto next = Pop();
			if (next)
			{
				auto& node = *next;
				if (policy.Visit(node))
				{
					auto&& input_span = policy.DeeperEdges(node);
					for (auto& edge : input_span)
					{
						auto&& info = policy.GetNodeInfo(node, edge);
						auto&& key = policy.GetKeyFromInfo(info);
						//Not opened
						if (opened_nodes.find(key) == opened_nodes.end())
						{
							//Add to list of nodes to visit
							Push(policy.GetNodeFromInfo(info));
						}
					}
				}
			}
			return next.operator bool();
		}

		TraverseType<Node> frontier;
		hash_set<Key> opened_nodes;
	};
}