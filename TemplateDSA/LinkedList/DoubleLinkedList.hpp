#pragma 
#include "myHead.h"

template<typename T>
class DoubleLinkedList {
protected:
	template<typename T>
	struct ListNode {//链表结构
		using Node = ListNode<T>;
		T element;
		Node* prev;
		Node* next;
		ListNode(const T& in, Node* inPrev = nullptr, Node* inNext = nullptr) :element(in), prev(inPrev), next(inNext) {};
		ListNode(T&& in, Node* inPrev = nullptr, Node* inNext = nullptr) :element(std::move(in)), prev(inPrev), next(inNext) {};
	};
	using Node = ListNode<T>;
	Node* head;  //链表头指针  
	Node* tail;  //链表尾指针  
	size_t length;//长度
public:
	//构造函数
	DoubleLinkedList(size_t initialSize = 0) :head(nullptr), tail(nullptr), length(initialSize) {
		if (!length) {
			return;
		}//size=length=4 head=0<->1<->2<->3
		head = new Node(T());//head=0
		Node* ptr = head;//i=1 tail=1 ptr=0 0->1 1->0 prev=1 ++i
		for (size_t i = 1; i < length; ++i) {//i=2 tail=2 ptr=1 1->2 2->1 ptr=2 ++i
			tail = new Node(T(),ptr);//i=3 tail=3 ptr=2 2->3 3->2 ptr=3 ++i
			ptr->next = tail;
			ptr = tail;
		}
	}
	//创建size个in元素
	DoubleLinkedList(size_t initialSize, const T& in) :head(nullptr), tail(nullptr), length(initialSize) {
		if (!length) {
			return;
		}//size=length=4 head=0<->1<->2<->3
		head = new Node(in);//head=0
		Node* ptr = head;//i=1 tail=1 ptr=0 0->1 1->0 prev=1 ++i
		for (size_t i = 1; i < length; ++i) {//i=2 tail=2 ptr=1 1->2 2->1 ptr=2 ++i
			tail = new Node(in, ptr);//i=3 tail=3 ptr=2 2->3 3->2 ptr=3 ++i
			ptr->next = tail;
			ptr = tail;
		}
	}
	//列表初始化
	DoubleLinkedList(std::initializer_list<T> in) :head(nullptr), tail(nullptr), length(in.size()) {
		if (!length) {
			return;
		}
		const T* it = in.begin();
		head = new Node(*it);
		Node* ptr = head;
		for (++it; it != in.end(); ++it) {
			tail = new Node(*it, ptr);
			ptr->next = tail;
			ptr = tail;
		}
	}
	//拷贝构造
	DoubleLinkedList(const DoubleLinkedList& other) :head(nullptr), tail(nullptr), length(other.length) {
		if (!length) {
			return;
		}
		head = new Node(other.head->element);
		Node* ptr = head;
		for (Node* tempother = other.head->next; tempother != nullptr; tempother = tempother->next) {
			tail = new Node(tempother->element, ptr);
			ptr->next = tail;
			ptr = tail;
		}
	}
	//移动构造
	DoubleLinkedList(DoubleLinkedList&& other)noexcept :head(other.head), tail(other.tail), length(other.length) {
		other.head = nullptr;
		other.tail = nullptr;
		other.length = NULL;
	}
	//移动构造
	~DoubleLinkedList() {
		clear();
	}
	//清空链表
	void clear() {
		_clearPtr(head,0);
	}
	//清空ptr以及之后或之前的所有链表
	void _clearPtr(Node* ptr,int model) {
		if (model == 0) {//正序清理
			tail = ptr->prev;
			for (Node* tempnext = nullptr; ptr != nullptr;) {//head=0<->1<->2<->3 ptr=1
				tempnext = ptr->next;//next=2 delete 1 ptr=next=2
				delete ptr;//next=3 delete 2  ptr=next=3
				ptr = tempnext;//next=null delete 3 ptr=null end
			}
		}
		else {
			head = ptr->next;
			for (Node* tempprev = nullptr; ptr != nullptr;) {//0<->1<->2<->3=tail ptr=2
				tempprev = ptr->prev;//prev=1 delete 2 ptr=prev=1
				delete ptr;//prev=0 delete 1 ptr=prev=0
				ptr = tempprev;//prev=null delete 0 ptr=null end
			}
		}
		_alignLength(model);
	}
	//拷贝赋值
	DoubleLinkedList& operator=(const DoubleLinkedList& other) {
		//清空链表
		if (this != &other) {
			clear();
			//拷贝链表
			length = other.length;
			if (!length) {
				return;
			}
			head = new Node(other.head->element);
			Node* ptr = head;
			for (Node* tempother = other.head->next; tempother != nullptr; tempother = tempother->next) {
				tail = new Node(tempother->element, ptr);
				ptr->next = tail;
				ptr = tail;
			}
		}
		return *this;
	}
	//移动赋值
	DoubleLinkedList& operator=(DoubleLinkedList&& other) noexcept {
		//清空链表
		if (this != &other) {
			clear();
			length = other.length;
			head = other.head;
			tail = other.tail;
			other.length = NULL;
			other.head = nullptr;
			other.tail = nullptr;
		}
		return *this;
	}
	//校准链表长度
	size_t _alignLength(int model) {//head=0<->1<->2
		length = 0;
		if (model == 0) {//正序矫正长度
			for (Node* ptr = head; ptr != nullptr; ++length) { ptr = ptr->next; }//ptr=0 i=0;ptr=1 i=1;ptr=2 i=2;ptr=null i=3;end
		}
		else {
			for (Node* ptr = tail; ptr != nullptr; ++length) { ptr = ptr->prev; }//ptr=2 i=0;ptr=1 i=1;ptr=0 i=2;ptr=null i=3;end
		}
		return length;
	}
	//返回链表长度
	size_t size() const {
		return length;
	}
	//如果链表为空 返回TRUE
	bool empty() const {
		return head == nullptr;
	}
	//反转链表
	void reverse() {
		if (head == nullptr || head->next == nullptr) {
			return; // 空链表或只有一个节点，无需反转
		}
		tail = head;//更新尾节点指针为原来链表的头节点
		Node* tempptr = nullptr;
		while (head->next != nullptr) {
			tempptr = head->next;
			head->next = head->prev;
			head->prev = tempptr;
			head = tempptr;
		}
		head->next = head->prev;
	}
	//返回指针指向元素
	static T& getElem(Node* ptr) {
		return ptr->element;
	}
	static const T& getElem(Node* ptr) const {
		return ptr->element;
	}
	//返回指针后继
	static Node* getNext(Node* ptr) {
		return ptr->next;
	}
	//返回指针前驱
	static Node* getPrev(Node* ptr) {
		return ptr->prev;
	}
	//访问第index链表指针
	Node* _atPtr(size_t index) {
		if (index >= length) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif
			return nullptr;
		}
		if (index + 1 <= length / 2) {//head=0-5-3-2-1-4=tail index=2 length=6
			Node* ptr = head;//i=0 ptr=0 ++i;i=1 ptr=5 ++i;i=2 ptr=3 end
			for (size_t i = 0; i < index || ptr != nullptr; ++i) { ptr = ptr->next; }
		}
		else {//head=0-5-3-2-1-4=tail index=3 length=6
			Node* ptr = tail;//i=5 ptr=4 --i;i=4 ptr=1 --i;i=3 ptr=2 end 
			for (size_t i = length - 1; i > index || ptr != nullptr; --i) { ptr = ptr->prev; }
		}
		return ptr;
	}
	//访问第index链表指针
	const Node* _atPtr(size_t index) const {
		if (index >= length) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif
			return nullptr;
		}
		if (index + 1 <= length / 2) {//head=0<->1<->2<->3<->4<->5=tail index=2 length=6
			Node* ptr = head;//i=0 ptr=1 ++i;i=1 ptr=2 ++i;i=2 end
			for (size_t i = 0; i < index || ptr != nullptr; ++i) { ptr = ptr->next; }
		}
		else {//head=0<->1<->2<->3<->4<->5=tail index=3 length=6
			Node* ptr = tail;//i=5 ptr=4 --i;i=4 ptr=3 --i;i=3 end 
			for (size_t i = length - 1; i > index || ptr != nullptr; --i) { ptr = ptr->prev; }
		}
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
	//在指定链表前驱插入元素
	//指定指针可能不是本链表指针,小心使用
	//属于内部函数,故没有对length修改
	Node* _insertPrevPtr(const Node*& ptr, const T& value) {
		if (ptr != nullptr) {
			Node* tempptr = Node(value);//2<->3<->4 ptr=3 value=new
			if(ptr->prev!=nullptr)
				ptr->prev->next = tempptr;//2->new
			tempptr->prev = ptr->prev;//2<-new
			tempptr->next = ptr;//new->3
			ptr->prev = tempptr;//new<-3
			return tempptr;
		}
	}
	//在指定链表后继插入元素
	//指定指针可能不是本链表指针,小心使用
	//属于内部函数,故没有对length修改
	Node* _insertNextPtr(const Node*& ptr, const T& value) {
		if (ptr != nullptr) {
			Node* tempptr = Node(value);//2<->3<->4 ptr=3 value=new
			if (ptr->next != nullptr)
				ptr->next->prev = tempptr;//new<-4
			tempptr->next = ptr->next;//new->4
			tempptr->prev = ptr;//3<-new
			ptr->next = tempptr;//3->new
			return tempptr;
		}
	}
	//在指定删除链表
	//指定指针可能不是本链表指针,小心使用
	void _removePtr(const Node*& ptr) {
		if (ptr != nullptr) {//2<->3<->4 ptr=3
			if(ptr->prev != nullptr)
				ptr->prev->next = ptr->next;//2->4
			if(ptr->next != nullptr)
				ptr->next->prev = ptr->prev;//2<-4
			delete ptr;
		}
	}
	//在指定位置插入元素
	void insert(size_t index, const T& value) {
		if (index > length) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif //DEBUG
			return;
		}
		if (index == 0 && length == 0) {
			head = new Node(value);
			tail = head;
		}
		else if (index == 0) {
			head->prev = new Node(value, nullptr, head);//head=0 Null<-new<->0
			head = head->prev;//head=new
		}
		else if (index == length) {
			tail->next = new Node(value, tail, nullptr);//tail=2 2<->new->Null
			tail = tail->next;//tail=new
		}
		else {
			_insertNextPtr(_atPtr(index - 1), value);
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
		Node* ptr = _atPtr(index);
		if (ptr == head) {
			head = head->next;
			head->prev = nullptr;
		}
		else if (ptr == tail) {
			tail = tail->prev;
			tail->next = nullptr;
		}
		_removePtr(ptr);
		--length;
	}
	//末尾插入元素
	void push_back(const T& value) {
		tail->next = new Node(value,tail,nullptr);//tail=2 2<->new->Null
		tail = tail->next;//tail=new
		++length;
	}
	//末尾移除元素
	void pop_back() {
		if (tail == nullptr) {
			length = 0;
			return;
		}
		if (head == tail) {
			delete tail;
			head = nullptr;
			tail = nullptr;
			length = 0; 
			return;
		}
		Node* ptr = tail->prev;
		ptr->next = nullptr;
		delete tail;
		tail = ptr;
		--length;
	}
	//首位添加元素
	void push_front(const T& value) {
		head->prev = new Node(value,nullptr, head);//head=0 Null<-new<->0
		head = head->prev;//head=new
		++length;
	}
	//首位移除元素
	void pop_front() {
		if (head == nullptr) {
			length = 0;
			return;
		}
		if (head == tail) {
			delete tail;
			head = nullptr;
			tail = nullptr;
			length = 0;
			return;
		}
		Node* ptr = head->next;
		ptr->prev = nullptr;
		delete head;
		head = ptr;
		--length;
	}
	//对所有元素执行func
	template<typename Function>
	void forEachFunction(Function func) {
		for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) {
			func(ptr->element);
		}
	}
	//对所有元素逆向执行func
	template<typename Function>
	void forEachFunctionInReverse(Function func) {
		for (Node* ptr = tail; ptr != nullptr; ptr = ptr->prev) {
			func(ptr->element);
		}
	}
	//寻找第一个符合cond的元素的索引
	template<typename Condition>
	Node* findFirstByCondition(Condition cond) {
		for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) {
			if (cond(ptr->element))
				return ptr;
		}
		return nullptr;
	}
	//寻找最后一个符合cond的元素的索引
	template<typename Condition>
	Node* findLostByCondition(Condition cond) {
		for (Node* ptr = tail; ptr != nullptr; ptr = ptr->prev) {
			if (cond(ptr->element))
				return ptr;
		}
		return nullptr;
	}
	//对符合cond元素执行func
	template<typename Condition, typename Function>
	void forEachFunctionByCondition(Condition cond, Function func) {
		for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) {
			if (cond(ptr->element))
				func(ptr->element);
		}
	}
	//逆序对符合cond元素执行func
	template<typename Condition, typename Function>
	void forEachFunctionByConditionInReverse(Condition cond, Function func) {
		for (Node* ptr = tail; ptr != nullptr; ptr = ptr->prev) {
			if (cond(ptr->element))
				func(ptr->element);
		}
	}
	//从index开始正序寻找与value相等元素,返回所在指针,如果没有返回nullptr
	Node* findPtrInOrder(const T& value, size_t index = 0) {//head=0-5-3-2-1-4 index=2 value=4
		for (Node* ptr = _atPtr(index); ptr != nullptr; ptr = ptr->next) {//i=2 ptr=3;i=3 ptr=2;i=4 ptr=1;i=5 ptr=4;end
			if (ptr->element == value) {
				break;
			}
		}
		return ptr;
	}
	//从index开始正序寻找与value相等元素,返回所在索引,如果没有返回length
	size_t findIndexInOrder(const T& value, size_t startIndex = 0) {//head=0-5-3-2-1-4 index=2 value=4
		for (Node* ptr = _atPtr(startIndex); ptr != nullptr; ptr = ptr->next, ++startIndex) {//i=2 ptr=3;i=3 ptr=2;i=4 ptr=1;i=5 ptr=4;end
			if (ptr->element == value) {
				break;
			}
		}
		return startIndex;
	}
	//从index开始逆向寻找与value相等元素,返回所在索引,如果没有返回length
	Node* findPtrInReverse(const T& value, size_t index = 0) {//head=0-5-3-2-1-4 index=3 value=3
		for (Node* ptr = _atPtr(index); ptr != nullptr; ptr = ptr->prev, --index) {//i=3 ptr=2;i=2 ptr=3;end
			if (ptr->element == value) {
				break;
			}
		}
		return ptr;
	}
	//从index开始逆向寻找与value相等元素,返回所在索引,如果没有返回length
	size_t findIndexInReverse(const T& value, size_t startIndex = 0) {//head=0-5-3-2-1-4 index=3 value=3
		for (Node* ptr = _atPtr(startIndex); ptr != nullptr; ptr = ptr->prev, --startIndex) {//i=3 ptr=2;i=2 ptr=3;end
			if (ptr->element == value) {
				break;
			}
		}//i=2 ptr=2;i=3 ptr=3;i=4 ptr=4;end
		return startIndex;
	}
};

