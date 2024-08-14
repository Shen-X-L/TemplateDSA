#pragma once
#include "myHead.h"

template<typename T>
class LinkedList {
protected:
	template<typename T>
	struct ListNode {//链表结构
		using Node = ListNode<T>;
		T element;
		Node* next;
		ListNode() :element() {};
		ListNode(const T& in, Node* ptr = nullptr) :element(in), next(ptr) {};
		ListNode(T&& in, Node* ptr = nullptr) :element(std::move(in)), next(ptr) {};
	};
	using Node = ListNode<T>;
	Node* head;  //链表头指针  
	size_t length;//长度
public:
	//构造函数
	LinkedList(size_t initialSize = 0) 
		:head(nullptr), length(initialSize) {
		if (!length) {
			return;
		}//size=length=3 head=0->1->2
		head = new Node(T());//head=0
		Node* prev = head;//前驱
		for (size_t i = 1; i < length; ++i) {//i=1 next=1 prev=0->1 prev=1 ++i
			prev->next = new Node(T());//i=2 next=2 prev=1->2 prev=2 ++i
			prev = prev->next;//i=3 end
		}
	}
	//创建size个in元素
	LinkedList(size_t initialSize, const T& in) 
		:head(nullptr), length(initialSize) {
		if (!length) {
			return;
		}//size=length=3 head=0->1->2
		head = new Node(in);//head=0
		Node* prev = head;//前驱
		for (size_t i = 1; i < length; ++i) {//i=1 next=1 prev=0->1 prev=1 ++i
			prev->next = new Node(T(in));//i=2 next=2 prev=1->2 prev=2 ++i
			prev = prev->next;//i=3 end
		}
	}
	//列表初始化
	LinkedList(std::initializer_list<T> in) 
		:head(nullptr), length(in.size()) {
		if (!length) {
			return;
		}
		const T* it = in.begin();
		head = new Node(*it);
		Node* prev = head;//前驱
		for (++it; it != in.end(); ++it) {
			prev->next = new Node(*it);
			prev = prev->next;
		}
	}
	//拷贝构造
	LinkedList(const LinkedList& other) 
		:head(nullptr), length(other.length) {
		if (other.head == nullptr) {
			length = 0;
			return;
		}
		head = new Node(other.head->element);
		Node* prev = head;//前驱
		for (Node* tempother = other.head->next; tempother != nullptr; tempother = tempother->next) {
			prev->next = new Node(tempother->element);
			prev = prev->next;
		}
	}
	//移动构造
	LinkedList(LinkedList&& other)noexcept 
		:head(other.head), length(other.length) {
		other.head = nullptr;
		other.length = 0;
	}
	//移动构造
	~LinkedList() {
		clear();
	}
	//清空链表
	inline void clear() {
		_clearPtr(head);
	}
	//清空ptr以及之后的所有链表
	void _clearPtr(Node* ptr) {
		Node* next = nullptr;//后继
		for (; ptr != nullptr;) {//head=0->1->2->3 ptr=1
			next = ptr->next;//next=2 delete 1 ptr=next=2
			delete ptr;//next=3 delete 2  ptr=next=3
			ptr = next;//next=null delete 3 ptr=null end
		}
		_alignLength();
	}
	//拷贝赋值
	LinkedList& operator=(const LinkedList& other) {
		if (this != &other){
			//清空链表
			clear();
			//拷贝链表
			if (other.head == nullptr) {
				length = 0;
				return *this;
			}
			length = other.length;
			head = new Node(other.head->element);
			Node* prev = head;//前驱
			for (Node* tempother = other.head->next; tempother != nullptr; tempother = tempother->next) {
				prev->next = new Node(tempother->element);
				prev = prev->next;
			}
		}
		return *this;
	}
	//移动赋值
	LinkedList& operator=(LinkedList&& other) noexcept {
		if (this != &other) {
			//清空链表
			clear();
			length = other.length;
			head = other.head;
			other.length = 0;
			other.head = nullptr;
		}
		return *this;
	}
	//校准链表长度 
	size_t _alignLength() {//head=0->1->2
		length = 0;
		for (Node* ptr = head; ptr != nullptr; ++length) { ptr = ptr->next; }//ptr=0 i=0;ptr=1 i=1;ptr=2 i=2;ptr=null i=3;end
		return length;
	}
	//返回链表长度
	inline size_t size() const {
		return length;
	}
	//如果链表为空 返回TRUE
	inline bool empty() const {
		return head == nullptr;
	}
	//反转链表
	void reverse() {
		if (head == nullptr || head->next == nullptr) {
			return; // 空链表或只有一个节点，无需反转
		}
		Node* prev = nullptr;
		Node* curr = head;
		Node* nextNode = nullptr;
		while (curr != nullptr) {
			nextNode = curr->next; // 保存下一个节点
			curr->next = prev; // 当前节点指向前一个节点，实现反转
			prev = curr; // 更新前一个节点为当前节点
			curr = nextNode; // 更新当前节点为下一个节点
		}
		head = prev; // 更新头指针为原来链表的尾节点
	}
	//返回指针指向元素
	inline static T& getElem(Node* ptr) {
		return ptr->element;
	}
	inline static const T& getElem(Node* ptr) const{
		return ptr->element;
	}
	//返回指针后继
	inline static Node* getNext(Node* ptr) {
		return ptr->next;
	}
	inline static const Node* getNext(Node* ptr) const {
		return ptr->next;
	}
	//访问第index链表指针
	Node* _atPtr(size_t index) {
		if (index >= length) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif
			return nullptr;
		}
		Node* ptr = head;//ptr=head=0->1->2->3 index=2
		for (size_t i = 0; i < index && ptr != nullptr; ++i) { ptr = ptr->next; }//i=0 ptr=1 ++i;i=1 ptr=2 ++i;i=2 end
		return ptr;
	}
	const Node* _atPtr(size_t index) const{
		if (index >= length) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif
			return nullptr;
		}
		Node* ptr = head;//ptr=head=0->1->2->3 index=2
		for (size_t i = 0; i < index && ptr != nullptr; ++i) { ptr = ptr->next; }//i=0 ptr=1 ++i;i=1 ptr=2 ++i;i=2 end
		return ptr;
	}
	//访问元素索引
	T& operator[](size_t index) {
		return  _atPtr(index)->element;
	}
	//访问元素索引
	const T& operator[](size_t index) const {
		return  _atPtr(index)->element;
	}
	//在指定指针的后继插入元素
	//指定指针可能不是本链表指针,小心使用
	//属于内部函数,故没有对length修改
	static Node* _insertNextPtr(const Node*& ptr, const T& value) {
		if (ptr != nullptr) {
			Node* next = new Node(value, ptr->next);
			ptr->next = next;
			return next;
		}
		return nullptr;
	}
	//在指定指针的后继删除元素
	//指定指针可能不是本链表指针,小心使用
	static Node* _removeNextPtr(const Node*& ptr) {
		if (ptr != nullptr && ptr->next != nullptr) {
			Node* next = ptr->next;
			ptr->next = next->next;
			delete next;
			return ptr;
		}
		return nullptr;
	}
	//在指定位置插入元素
	//在0-1-2 输入(3 new) 0-1-2-new
	void insert(size_t index, const T& value) {
		if (index > length) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif //DEBUG
			return;
		}
		if (index) {
			Node* prev = _atPtr(index - 1);
			if (prev != nullptr) {
				_insertNextPtr(prev, value);
			}
			else {
#ifdef DEBUG
				throw std::out_of_range("Invalid index");// 处理 prev 为 nullptr 的情况
#endif
				return;
			}
		}
		else {
			Node* ptr = new Node(value,head);//head=0->1  new->0->1
			head = ptr;//head=new->0->1
		}
		++length;
	}
	//删除指定索引位置的元素  
	void remove(size_t index) {
		if (index >= length) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif //DEBUG
			return;
		}
		if (index) {
			Node* prev = _atPtr(index - 1);
			if (prev != nullptr) {
				_removeNextPtr(prev, value);
			}
			else {
#ifdef DEBUG
				throw std::out_of_range("Invalid index");// 处理 prev 为 nullptr 的情况
#endif
				return;
			}
		}
		else {
			Node* ptr = head;//head=ptr=0->1->2
			head = ptr->next;//head=1->2 ptr=0
			delete ptr;
		}
		--length;
	}
	//末尾插入元素
	void push_back(const T& value) {
		Node* prev;
		for (prev = head; prev->next != nullptr; prev = prev->next) {};
		prev->next = new Node(value);
		++length;
	}
	//末尾移除元素
	void pop_back() {
		if (head == nullptr) {
			length = 0;
			return;
		}
		if (head->next == nullptr) {
			delete head;
			length = 0;
			return;
		}
		Node* prev;
		for (prev = head; prev->next->next != nullptr; prev = prev->next) {};
		delete prev->next;
		prev->next = nullptr;
		--length;
	}
	//首位添加元素
	void push_front(const T& value) {
		Node* ptr = head;
		head = new Node(value);
		head->next = ptr;
		++length;
	}
	//首位移除元素
	void pop_front() {
		if (head == nullptr) {
			length = 0;
			return;
		}
		Node* ptr = head->next;
		delete head;
		head = ptr;
		--length;
	}
	//对所以元素执行func
	template<typename Function>
	inline void forEachFunction(Function func) {
		for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) {
			func(ptr->element);
		}
	}
	//寻找第一个符合cond的元素的索引
	template<typename Condition>
	inline Node* findFirstByCondition(Condition cond) {
		for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) {
			if (cond(ptr->element))
				return ptr;
		}
		return nullptr;
	}
	//对符合cond元素执行func
	template<typename Condition, typename Function>
	inline void forEachFunctionByCondition(Condition cond, Function func) {
		for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) {
			if (cond(ptr->element))
				func(ptr->element);
		}
	}
	//从index开始寻找与value相等元素,返回所在指针,如果没有返回nullptr
	inline Node* findPtrInOrder(const T& value, size_t index = 0) {//head=0-1-2-3-4-5 index=2 value=4
		for (Node* ptr = _atPtr(index); ptr != nullptr; ptr = ptr->next;) {//i=2 ptr=2;i=3 ptr=3;i=4 ptr=4;end
			if (ptr->element == value) {
				return ptr;
			}
		}
		return nullptr;
	}
	//从index开始寻找与value相等元素,返回所在索引,如果没有返回length
	inline size_t findIndexInOrder(const T& value, size_t startIndex = 0) {//head=0-1-2-3-4-5 index=2 value=4
		for (Node* ptr = _atPtr(startIndex); ptr != nullptr && startIndex < length; ptr = ptr->next, ++startIndex;) {//i=2 ptr=2;i=3 ptr=3;i=4 ptr=4;end
			if (ptr->element == value) {
				return startIndex;
			}
		}
		return length;
	}
};
