#pragma once
#include "myHead.h"

template<typename T>
class LinkStack {
protected:
	template<typename T>
	struct ListNode {//链表栈结构
		using Node = ListNode<T>;
		T element;
		Node* next;
		ListNode(const T& in, Node* ptr = nullptr) :element(in), next(ptr) {};
		ListNode(T&& in, Node* ptr = nullptr) :element(std::move(in)), next(ptr) {};
	};
	using Node = ListNode<T>;
	Node* stackTop = nullptr;
	//top 2->1->0->NULL
public:
	//构造函数
	LinkStack() {};
	//创建size个in元素
	LinkStack(size_t initialSize, const T& in) {//top=2->1->0
		Node* prev = nullptr;//后继
		for (size_t i = 0; i < initialSize; ++i) {//i=0 top=prev=0->Null ++i
			prev = new Node(in, stackTop);//i=1 top=prev=1->0 ++i
			stackTop = prev;//i=2 top=prev=2->1->0 ++i
		}
	}
	//列表初始化
	LinkStack(std::initializer_list<T> in) {//top=2->1->0 {0,1,2}
		Node* prev = nullptr;//后继
		for (const T* it = in.begin(); it != in.end(); ++it) {//i=0 top=prev=0->Null ++i
			prev = new Node(*it, stackTop);//i=1 prev=1->0 top=1->0 ++i
			stackTop = prev;//i=2 prev=2->1 top=2->1->0 ++i
		}
	}
	//拷贝构造
	LinkStack(const LinkStack& other) {
		if (other.stackTop != nullptr) {
			stackTop = new Node(other.stackTop->element);
			Node* prev = stackTop;
			for (Node* tempother = other.stackTop->next; tempother != nullptr; tempother = tempother->next) {
				prev->next = new Node(tempother->element);
				prev = prev->next;
			}
		}
	}
	//移动构造
	LinkStack(LinkStack&& other)noexcept :stackTop(other.stackTop) {
		other.stackTop = nullptr;
	}
	//析构函数
	~LinkStack() {
		clear();
	}
	//清空链表栈
	void clear() {
		_clearPtr(stackTop);
	}
	//清空ptr以及之后的所有链表栈
	static void _clearPtr(Node* ptr) {
		for (Node* next = nullptr; ptr != nullptr; ptr = next) {//top=0->1->2->3 ptr=1
			next = ptr->next;//next=2 delete 1 ptr=next=2
			delete ptr;//next=3 delete 2  ptr=next=3
		}//next=null delete 3 ptr=null end
	}
	//拷贝赋值
	LinkStack& operator=(const LinkStack& other) {
		if (this != &other) {
			//清空链表栈
			clear();
			//拷贝链表栈
			if (other.stackTop != nullptr) {
				stackTop = new Node(other.stackTop->element);
				Node* prev = stackTop;
				for (Node* tempother = other.stackTop->next; tempother != nullptr; tempother = tempother->next) {
					prev->next = new Node(tempother->element);
					prev = prev->next;
				}
			}
		}
		return *this;
	}
	//移动赋值
	LinkStack& operator=(LinkStack&& other) noexcept {
		if (this != &other) {
			//清空链表栈
			clear();
			stackTop = other.stackTop;
			other.stackTop = nullptr;
		}
		return *this;
	}
	//返回栈长度
	size_t size() const {
		size_t length = 0;//0->1->2
		for (Node* ptr = stackTop; ptr != nullptr; ++length) { ptr = ptr->next; }//ptr=0 i=0;ptr=1 i=1;ptr=2 i=2;ptr=null i=3;end
		return length;
	}
	//如果栈为空 返回TRUE
	inline bool empty() const {
		return stackTop == nullptr;
	}
	//返回栈顶元素
	T& top() {
		if (empty()) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif //DEBUG
			return nullRef<T>();
		}
		return stackTop->element;
	}
	//返回栈顶元素
	const T& top() const {
		if (empty()) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif //DEBUG
			return nullRef<T>();
		}
		return stackTop->element;
	}
	//入栈
	void push(const T& value) {
		stackTop = new Node(value, stackTop);
		return;
	}
	//出栈
	void pop() {
		if (stackTop == nullptr) {
			return;
		}
		Node* ptr = stackTop->next;
		delete stackTop;
		stackTop = ptr;
	}
	//对所以元素执行func
	template<typename Function>
	inline void forEachFunction(Function func) {
		for (Node* ptr = stackTop; ptr != nullptr; ptr = ptr->next) {
			func(ptr->element);
		}
	}
};


