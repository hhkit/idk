#pragma once
#include <idk.h>
#include <ds/index_span.h>
namespace idk
{
    namespace graph_theory
    {

        struct IntermediateGraph
        {
            using index_t = size_t;
            vector<index_t> in_list;
            //Outgoing edges
            vector<index_span> adjacency_list;
            vector<index_t> adjacency_buffer;
            IntermediateGraph(size_t num_nodes) :in_list(num_nodes), adjacency_list(num_nodes) {}
            void ClearAdjacencyList()
            {
                adjacency_list.clear();
                adjacency_buffer.clear();
            }
            span<const index_t> AdjacencyList(index_t node)const
            {
                auto [begin, end] = adjacency_list[node];
                return { adjacency_buffer.data() + begin,adjacency_buffer.data() + end };
            }
            //Can only set new nodes.
            template<typename Itr>
            void SetAdjacentNodes(index_t src, Itr index_begin, Itr index_end)
            {
                assert(adjacency_list[src]._begin== adjacency_list[src]._end && adjacency_list[src]._end == 0);
                auto start = adjacency_buffer.size();
                auto len = std::distance(index_begin, index_end);
                adjacency_buffer.reserve(start + len);
                while (index_begin != index_end)
                {
                    auto dst = *index_begin;
                    adjacency_buffer.emplace_back(dst);
                    ++in_list[dst];
                    ++index_begin;
                }
                auto end = adjacency_buffer.size();
                adjacency_list[src] = index_span{ start,end };
            }

        };
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
        //returns a pair of vector of indices and a boolean to see if it succeeded(failure means it's cyclic)
        using index_t = size_t;
        std::pair<vector<index_t>, bool> KahnsAlgorithm(IntermediateGraph graph);
    }
}