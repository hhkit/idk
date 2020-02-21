#include "pch.h"
#include "topological_sort.h"

/*
template<typename T, typename = decltype(std::declval<T>().begin())>
std::ostream& operator<<(std::ostream& o, T& cont)
{
o << "{ ";
for (auto& obj : cont)
{
o << obj << ", ";
}
return o << " }";
}
*/
//Conducts a topological sort on the graph so that all nodes that lead into any particular node have an index larger than the particular node.
namespace idk::graph_theory
{

#pragma	optimize("",off)
//returns a pair of vector of indices and a boolean to see if it succeeded(failure means it's cyclic)
std::pair<vector<index_t>, bool> KahnsAlgorithm(IntermediateGraph graph)
{
	auto in_list = std::move(graph.in_list);
	std::pair<vector<index_t>, bool>  result;
	auto& sorted_indices = result.first;
	vector<index_t> stack{};
	stack.reserve(in_list.size());
	sorted_indices.reserve(in_list.size());
	//Find all nodes with 0 input dependencies
	for (size_t i = 0; i < in_list.size(); ++i)
	{
		if (in_list[i] == 0)
		{
			stack.emplace_back(i);
		}
	}


	while (!stack.empty())
	{
		//Take a node with 0 dependencies, and pop it off the stack
		auto node_index = stack.back();
		stack.pop_back();

		sorted_indices.emplace_back(node_index);

		//remove all outgoing edges(subtracting incoming count for related nodes)
		for (auto& adj_index : graph.AdjacencyList(node_index))
		{
			//If there are no nodes leading in anymore, add to stack of nodes with 0 dependencies
			if (--in_list[adj_index] == 0)
			{
				stack.emplace_back(adj_index);
			}
		}
	}
	bool& done = result.second = true;
	//When no nodes with 0 dependencies are left in the stack, any nodes with incoming edges remaining have some kind of circular dependencies
	for (auto num_in_edges : in_list)
	{
		done &= !(bool)num_in_edges;
	}
	return result;
}

}