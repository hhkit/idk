#include <unique>
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
		void emplace_back(Args&& ... args, const int tid);
		std::unique_ptr<T> pop_front(const int tid);
	private:
		struct Node;
		
		alignas(cache_line_sz) std::atomic<Node*> head{nullptr};
		alignas(cache_line_sz) std::atomic<Node*> tail{nullptr};
		HazardPointerDL<Node> hazards;
		
		bool casTail(Node* tail, Node* new_tail) { return tail.compare_exchange_strong(tail, new_tail);};
		bool casHead(Node* head, Node* new_head) { return head.compare_exchange_strong(head, new_head);};
	};
	
	template<typename T>
	struct queue<T>::Node
	{
		std::unique_ptr<T> data;
		std::atomic<Node*> next{nullptr};
		std::atomic<Node*> prev{nullptr};
		template<typename ... Args>
		Node(Args&& ...args) : data{std::make_unique<T>(std::forward<Args>(args)...)}{}
	};
	
	template<typename T>
	queue<T>::queue()
	{
		auto* sentinel = new Node{};
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
	void queue<T>::emplace_back(Args&& ... args, const int tid)
	{
		auto new_node = new Node{std::forward<Args>(args)...)};
		
		while (true)
		{
			auto ltail = hazards.protect(tail);
			auto prev = ltail->prev;
			new_node->prev = ltail;
			// if the previous enqueue is done
			if (prev->next.load() == nullptr)
				prev->next.store(ltail, std::memory_order_relaxed);
			
			if (castTail(tail, new_node))
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
		const auto tid = 0;
		
		while (true)
		{
			auto lhead = hazards.protect(head.load(), tid);
			if (lhead != head.load()) // busy waiting until head is safe to change
				continue; 
			auto next = lhead->next.load();
			if (lnext == nullptr)
			{
				hazards.clear(tid);
				return {};
			}
			
			if (casHead(lhead, lnext))
			{
				auto retval = std::move(lnext->data);
				hp.clear(tid);
				hp.free(lhead, tail.load(), tid);
				return retval;
			}
		}
	}
}