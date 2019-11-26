#include <memory>
#include <machine.h>
#include <parallel/HazardPointerDL.h>

namespace idk::mt
{
	template<typename T>
	class queue
	{
	public:
		queue();
		~queue();
		
		template<typename ... Args>
		void emplace_back(const int tid, Args&& ... args);
		std::unique_ptr<T> pop_front(const int tid);
	private:
		struct Node;
		
		alignas(machine::cache_line_sz) std::atomic<Node*> head{nullptr};
		alignas(machine::cache_line_sz) std::atomic<Node*> tail{nullptr};
		HazardPointerDL<Node> hazards;
		
		bool casTail(Node* ltail, Node* new_tail) { return tail.compare_exchange_strong(ltail, new_tail);};
		bool casHead(Node* lhead, Node* new_head) { return head.compare_exchange_strong(lhead, new_head);};
	};
	
	template<typename T>
	struct queue<T>::Node
	{
		std::unique_ptr<T> data;
		Node* prev{ nullptr };
		std::atomic<Node*> next{nullptr};
		template<typename ... Args>
		Node(Args&& ...args) : data{std::make_unique<T>(std::forward<Args>(args)...)}{}
	};
	
	template<typename T>
	queue<T>::queue()
	{
		auto* sentinel = new Node{};
		sentinel->prev = sentinel;
		head.store(sentinel);
		tail.store(sentinel);
	}
	
	template<typename T>
	queue<T>::~queue()
	{
		while(pop_front(0) != nullptr); // pop everything
		delete head.load();           // delete sentinel 
	}
	
	template<typename T>
	template<typename ... Args>
	void queue<T>::emplace_back(const int tid, Args&& ... args)
	{
		auto new_node = new Node{std::forward<Args>(args)...};
		IDK_ASSERT(new_node->data);
		IDK_ASSERT(*new_node->data);
		while (true)
		{
			auto ltail = hazards.protect(tail.load(), tid);
			if (ltail != tail.load())
				continue;
			auto prev = ltail->prev;
			new_node->prev = ltail;
			// if the previous enqueue is done
			/*if (prev->next.load() == nullptr)
				prev->next.store(ltail, std::memory_order_relaxed);*/
			
			if (casTail(ltail, new_node))
			{
				// make sure that everyone knows the next tail is set
				ltail->next.store(new_node, std::memory_order_release);
				hazards.clear(tid);
				return;
			}
		}
	}
	
	template<typename T>
	std::unique_ptr<T> queue<T>::pop_front(const int tid)
	{
		while (true)
		{
			auto lhead = hazards.protect(head.load(), tid);
			if (lhead != head.load()) // busy waiting until head is safe to change
				continue; 
			auto lnext = lhead->next.load();
			if (lnext == nullptr)
			{
				hazards.clear(tid);
				return {};
			}
			
			if (casHead(lhead, lnext))
			{
				auto retval = std::move(lnext->data);
				IDK_ASSERT(retval);
				hazards.clear(tid);
				hazards.free(lhead, tail.load(), tid);
				return retval;
			}
		}
	}
}