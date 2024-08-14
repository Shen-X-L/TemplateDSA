#pragma once
#include "myHead.h"

template<typename T>
class StaticDoubleLinkedList {//静态表
	/*
	id	elem	next	prev
	0	Nul		5		4		备用链表
	1	a		3		Nul		使用链表
	2	b		Nul		3
	3	c		2		1
	4	Nul		0		5
	5	Nul		4		0
	1-3-2-null	0-5-4-null
	*/
protected:
	template<typename T>
	struct ListNode {
		using Node = ListNode<T>;
		T element;
		size_t next;
		size_t prev;
		ListNode() :next(NULL), prev(NULL) {};
		ListNode(const T& in, size_t inNext = NULL, size_t inPrev = NULL) :element(in), next(inNext),prev(inPrev) {};
		ListNode(T&& in, size_t inNext = NULL, size_t inPrev = NULL) :element(std::move(in)), next(inNext), prev(inPrev) {};
		ListNode(const Node& other) :element(other.element), next(other.next), prev(other.prev) {};
		ListNode(Node&& other) :element(std::move(other.element)), next(other.next), prev(other.prev) {};
		Node& operator=(const Node& other) {
			element = other.element;
			next = other.next;
			prev = other.prev;
		}
		Node& operator=(Node&& other) {
			element = std::move(other.element);
			next = other.next;
			prev = other.prev;
		}
	};
	using Node = ListNode<T>;
	size_t length;//元素个数
	size_t capacity;//分配内存长度 
	size_t head;//首元素位置 如果为0代表空表
	size_t tail;//尾元素位置
	Node* list;
public:
	//构造函数
	/*
	0	Nul	1	3
	1	Nul	2	0
	2	Nul	3	1
	3	Nul	0	2
	*/
	StaticDoubleLinkedList(size_t initialSize = 0)
		:length(0), capacity(initialSize ? initialSize + 1 : 0), head(0), tail(0), list(capacity ? new T[capacity] : nullptr) {
		if (list) {
			for (size_t i = 0; i < capacity; ++i) {//size=3 capacity=4
				list[i].prev = i - 1;
				list[i].next = i + 1;
			}
			list[0].prev = capacity - 1;
			list[capacity - 1].next = NULL;
		}
	}
	//创建size个in元素 
	/*
	length=3 capacity=4
	0	Nul	0	0	
	1	in	2	Nul	
	i=1
	0	Nul	0	0
	1	in	2	Nul	
	2	in	3	1
	i=2
	0	Nul	0	0
	1	in	2	Nul	
	2	in	3	1
	3	in	4	2
	end
	3	in	Nul	2
	*/
	StaticDoubleLinkedList(size_t initialSize, const T& in)
		:length(initialSize), capacity(initialSize ? initialSize + 1 : 0), head(0), tail(length), list(capacity ? new T[capacity] : nullptr) {
		if (list) {
			head = 1;
			for (size_t i = 1; i <= length; ++i) {
				list[i].next = i + 1;
				list[i].prev = i - 1;
				list[i].element = in;
			}
			list[tail].next = NULL;
		}
	}
	//列表初始化
	//同上
	StaticDoubleLinkedList(std::initializer_list<T> in) 
		:length(in.size()), capacity(in.size() ? in.size() + 1 : 0), head(0), tail(length), list(capacity ? new T[capacity] : nullptr) {
		if (list) {
			head = 1;
			size_t i = 1;
			for (const T* it = in.begin(); it != in.end(); ++it, ++i)) {
				list[i].next = i + 1;
				list[i].prev = i - 1;
				list[i].element = in;
			}
			list[tail].next = NULL;
		}
	}
	//拷贝构造
	StaticDoubleLinkedList(const StaticDoubleLinkedList& other)
		:length(other.length), capacity(other.capacity), head(other.head), tail(other.tail), list(new Node[capacity]) {
		for (size_t i = 0; i < capacity; ++i) {
			list[i] = other.list[i];
		}
	}
	//移动构造
	StaticDoubleLinkedList(StaticDoubleLinkedList&& other) noexcept
		:length(other.length), capacity(other.capacity), head(other.head), tail(other.tail), list(other.list) {
		other.length = 0;
		other.capacity = 0;
		other.head = 0;
		other.tail = 0;
		other.list = nullptr;
	}
	//析构函数
	~StaticDoubleLinkedList() {
		delete[] list;
	}
	//拷贝赋值
	StaticDoubleLinkedList<T>& operator=(const StaticDoubleLinkedList& other) {
		if (this != &other) {
			delete[] list;
			length = other.length;
			capacity = other.capacity;
			head = other.head;
			tail = other.tail;
			list = new Node[capacity];
			for (size_t i = 0; i < capacity; ++i) {
				list[i] = other.list[i];
			}
		}
		return *this;
	}
	//移动赋值
	StaticDoubleLinkedList<T>& operator=(StaticDoubleLinkedList&& other) noexcept {
		if (this != &other) {
			delete[] list;
			list = other.list;
			length = other.length;
			capacity = other.capacity;
			head = other.head;
			tail = other.tail;
			other.list = nullptr;
			other.length = 0;
			other.capacity = 0;
			other.head = 0;
			other.tail = 0;
		}
		return *this;
	}
	//校准链表长度 
	/*  head=1
	0 3 length=3
	1 4 length=0
	2 N length=2
	3 N
	4 2 length=1
	*/
	size_t _alignLength() {
		length = 0;
		for (size_t ptr = head; ptr != NULL; ++length) { ptr = list[ptr].next; }
		return length;
	}
	//链表检查
	//校准链表长度 
	//检查是否存在循环链表
	//正序或倒序遍历为基准 检查前驱或后继是否正确
	//回收无关联链表
	//set控制以正序或倒序遍历为基准
	int _collectUnusedMemory(int set) {
		int error;//如果出错 返回错误类型 以后再写
		size_t templength = 0;
		bool* isNode = new bool[capacity]();//虽然想进行对bool[]类型压缩内存 目前先搁置
		if (set == 1) {//正序
			size_t i = head;
			for (size_t prev = NULL; i != NULL; i = list[i].next) {//如果i==NULL表遍历完成
				if (list[i].prev != prev) {
					error |= 0x1;//前驱不对齐
					list[i].prev = prev;
				}
				prev = i;
				++templength;
				isNode[i] = 1;
				if (isNode[list[i].next] == 1) {//isNude[list[i].next]==1表进入循环 跳出 
					list[i].next = NULL;//截断循环节
					error |= 0x2;
					break;
				}
			}
			if (i != tail) {//尾部未对齐
				tail = i;
				error |= 0x4;
			}
		}
		else {
			size_t i = tail;
			for (size_t next = NULL; i != NULL; i = list[i].prev) {//如果i==NULL表遍历完成
				if (list[i].next != next) {
					error |= 0x10;//后继不对齐
					list[i].next = next;
				}
				next = i;
				++templength;
				isNode[i] = 1;
				if (isNode[list[i].prev] == 1) {//isNude[list[i].next]==1表进入循环 跳出 
					list[i].prev = NULL;//截断循环节
					error |= 0x20;
					break;
				}
			}
			if (i != head) {//头部未对齐
				head = i;
				error |= 0x40;
			}
		}
		if (length != templength) {//长度未对齐
			length = templength;
			error |= 0x100;
		}
		/*
		应回收2 3 5
		ind	nex	pre
		i=2	coll=0
		0	2	X	
		2	X	0
		i=3	coll=2
		0	2	X
		2	3	0
		3	X	2
		i=5	coll=3
		0	2	X
		2	3	0
		3	5	2
		5	X	3
		coll=5
		0	2	5
		5	0	3
		*/
		size_t collect = 0;
		for (size_t i = 1; i < capacity; ++i) {
			if (!isNode[i]) {
				list[collect].next = i;
				list[i].prev = collect;
				collect = i;
			}
		}
		list[collect].next = 0;
		list[0].prev = collect;
		return error;
	}
	//链表重排序 
	//内存重分配
	//长度校准 无循环链表
	/*
	length = 3 head = 1
	this			temp
	ind	ele	nex	pre	ind	ele	nex	pre	(这里move后的next成员不知道销不销毁,默认为销毁,实际不影响逻辑)
	0	X	3	5	0	X	Nul	Nul
	1	a	4	Nul	1	X	Nul	Nul
	2	c	Nul	4	2	X	Nul	Nul
	3	X	6	0	3	X	Nul	Nul
	4	b	2	1	4	X	Nul	Nul
	5	X	0	6	5	X	Nul	Nul
	6	X	5	3	6	X	Nul	Nul
	temp
	1	a	2	0	i=4		j=2
	2	b	3	1	i=2		j=3
	3	c	4	2	i=Null	j=4
	end
	0	X	4	Nul	j=4
	4	X	5	3	j=5
	5	X	6	4	j=6
	6	X	7	5	j=7
	end
	0	X	4	6
	4	X	5	0	
	6	X	0	5	
	*/
	void _reorderAndResizeCapacity(size_t newCapacity) {
		if (newCapacity <= length) {
#ifdef DEBUG
			throw std::out_of_range("Capacity less than length");
#endif //DEBUG
			return;
		}
		Node* newList = new Node[newCapacity];
		size_t j = 1;
		for (size_t i = head; i != NULL || j <= length; ++j) {
			newList[j] = std::move(list[i]);
			newList[j].prev = j - 1;
			newList[j].next = j + 1;
			i = list[i].next;
		}
		newList[length].next = NULL;
		if (length + 1 != newCapacity) {
			newList[0].next = j;
			newList[0].next = newCapacity - 1;
			for (; j < newCapacity; ++j) {
				newList[j].next = j + 1;
				newList[j].prev = j - 1;
			}
			newList[newCapacity - 1].next = 0;
			newList[length + 1].prev = 0;
		}
		delete[] list;
		list = newList;
		head = 1;
		tail = length;
		capacity = newCapacity;
		return;
	}
	//返回链表长度
	size_t size() const {
		return length;
	}
	//如果链表为空 返回TRUE
	bool empty() const {
		if (head == NULL && tail == NULL) {
			return true;
		}
		if (head == NULL || tail == NULL) {//仅一头为空,检测链表
			_collectUnusedMemory(1);
			return empty;
		}
		else
			return false;
	}
	//返回指针指向元素
	T& getElem(size_t ptr) {
		return list[ptr].element;
	}
	const T& getElem(size_t ptr) const {
		return list[ptr].element;
	}
	//返回指针后继
	size_t getNext(size_t ptr) {
		return list[ptr].next;
	}
	//返回指针前驱
	size_t getPrev(size_t ptr) {
		return list[ptr].prev;
	}
	//访问第index链表指针
	/*
	head=1 length=4 index=2
	0 3
	1 4 ptr i=0
	2 5 ptr i=2
	3 N
	4 2 ptr i=1
	5 N
	*/
	size_t _atPtr(const size_t& index) {
		if (index >= length) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif
			return NULL;
		}
		if (index + 1 <= length / 2) {//head=0-5-3-2-1-4=tail index=2 length=6
			size_t ptr = head;//i=0 ptr=0 ++i;i=1 ptr=5 ++i;i=2 ptr=3 end
			for (size_t i = 0; i < index || ptr != NULL; ++i) { ptr = list[ptr].next; }
		}
		else {//head=0-5-3-2-1-4=tail index=3 length=6
			size_t ptr = tail;//i=5 ptr=4 --i;i=4 ptr=1 --i;i=3 ptr=2 end 
			for (size_t i = length - 1; i > index || ptr != NULL; --i) { ptr = list[ptr].prev; }
		}
		return ptr;
	}
	//访问第index链表指针
	const size_t _atPtr(const size_t& index) const {
		if (index >= length) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif
			return NULL;
		}
		if (index + 1 <= length / 2) {//head=0<->1<->2<->3<->4<->5=tail index=2 length=6
			size_t ptr = head;//i=0 ptr=1 ++i;i=1 ptr=2 ++i;i=2 end
			for (size_t i = 0; i < index || ptr != NULL; ++i) { ptr = list[ptr].next; }
		}
		else {//head=0<->1<->2<->3<->4<->5=tail index=3 length=6
			size_t ptr = tail;//i=5 ptr=4 --i;i=4 ptr=3 --i;i=3 end 
			for (size_t i = length - 1; i > index || ptr != NULL; --i) { ptr = list[ptr].prev; }
		}
		return ptr;
	}
	//访问元素索引
	T& operator[](size_t index) {
		return  list[_atPtr(index)].element;
	}
	//访问元素索引
	const T& operator[](size_t index) const {
		return  list[_atPtr(index)].element;
	}
	//在指定位置的前驱插入元素
	size_t _insertPrevPtr(const size_t& ptr, const T& value) {
		if (list[0].next == NULL || capacity < 2) {//无多余空间
			size_t newCapacity = capacity < 2 ? 2 : capacity * 2;
			Node* newList = new Node[newCapacity];
			for (size_t i = 0; i < capacity; ++i) { 
				newList[i] = std::move(list[i]); 
			}
			delete[] list;
			list = newList;
			/*
			cap=5 Newcap=8	ind	nex	pre	 中间链无修改
			0	X	3;	3	0	X;	5	6	4;	6	7	5;	7	8	6
			0	X	7;	3	5	X;	5	6	3;	6	7	5;	7	0	6
			*/
			for (size_t i = capacity; i < newCapacity; ++i) {
				newList[i - 1].next = i;
				newList[i].prev = i - 1;
			}
			list[list[0].prev].next = capacity;
			list[capacity].prev = list[0].prev;
			list[0].prev = newCapacity;
			list[newCapacity - 1].next = 0;
			capacity = newCapacity;
		}
		if (ptr == head) {//在头部插入
			/*
			list index=head=1
			0	F	3	6
			1	a	4	N
			2	c	5	4
			3	F	7	0
			4	b	2	1
			5	d	N	2
			6	F	0	7
			7	F	6	3

			0	F	7_3	6
			1	a	4	3_7
			3_1	new	1_5	N_6
			7	F	6	0_4
			*/
			size_t temp = list[0].next;
			list[temp].element = value;
			list[0].next = list[temp].next;
			list[list[temp].next].prev = 0;
			list[temp].next = head;
			list[temp].prev = NULL;
			list[head].prev = temp;
			head = temp;
			++length;
			return head;
		}
		else if(ptr == NULL){//在尾部插入
			/*
			list tail=5 index=0
			0	F	3	6
			1	a	4	N
			2	c	5	4
			3	F	7	0
			4	b	2	1
			5	d	N	2
			6	F	0	7
			7	F	6	3

			0	F	7_3	6
			5	d	3_7	2
			3_1	new	N_6	5_5
			7	F	6	0_4
			*/
			size_t temp = list[0].next;
			list[temp].element = value;
			list[0].next = list[temp].next;
			list[list[temp].next].prev = 0;
			list[temp].prev = tail;
			list[temp].next = NULL;
			list[tail].next = temp;
			tail = temp;
			++length;
			return tail;
		}
		else {
			/*
			list index=4 head=1 tail=5
			0	F	3	6
			1	a	4	N
			2	c	5	4
			3	F	7	0
			4	b	2	1
			5	d	N	2
			6	F	0	7
			7	F	6	3

			0	F	7_3	6
			1	a	3_7	N
			3_1	new	4_5	1_6
			4	b	2	3_8
			7	F	6	0_4
			*/
			size_t temp = list[0].next;
			list[temp].element = value;
			list[0].next = list[temp].next;
			list[list[temp].next].prev = 0;
			list[temp].next = ptr;
			list[temp].prev = list[ptr].prev;
			list[list[temp].prev].next = temp;
			list[ptr].prev = temp;
			++length;
			return temp;
		}
	}
	//在指定位置的后继插入元素
	size_t _insertNextPtr(const size_t& ptr, const T& value) {
		if (list[0].next == NULL || capacity < 2) {//无多余空间
			size_t newCapacity = capacity < 2 ? 2 : capacity * 2;
			Node* newList = new Node[newCapacity];
			for (size_t i = 0; i < capacity; ++i) {
				newList[i] = std::move(list[i]);
			}
			delete[] list;
			list = newList;
			/*
			cap=5 Newcap=8	ind	nex	pre	 中间链无修改
			0	X	3;	3	0	X;	5	6	4;	6	7	5;	7	8	6
			0	X	7;	3	5	X;	5	6	3;	6	7	5;	7	0	6
			*/
			for (size_t i = capacity; i < newCapacity; ++i) {
				newList[i - 1].next = i;
				newList[i].prev = i - 1;
			}
			list[list[0].prev].next = capacity;
			list[capacity].prev = list[0].prev;
			list[0].prev = newCapacity;
			list[newCapacity - 1].next = 0;
			capacity = newCapacity;
		}
		if (ptr == tail) {//在尾部插入
			/*
			list index=tail=5
			0	F	3	6
			1	a	4	N
			2	c	5	4
			3	F	7	0
			4	b	2	1
			5	d	N	2
			6	F	0	7
			7	F	6	3

			0	F	7_3	6
			5	d	3_7	2
			3_1	new	N_6	5_5
			7	F	6	0_4
			*/
			size_t temp = list[0].next;
			list[temp].element = value;
			list[0].next = list[temp].next;
			list[list[temp].next].prev = 0;
			list[temp].prev = tail;
			list[temp].next = NULL;
			list[tail].next = temp;
			tail = temp;
			++length;
			return tail;
		}
		else if (ptr == NULL) {//在头部插入
			/*
			list index=head=1
			0	F	3	6
			1	a	4	N
			2	c	5	4
			3	F	7	0
			4	b	2	1
			5	d	N	2
			6	F	0	7
			7	F	6	3

			0	F	7_3	6
			1	a	4	3_7
			3_1	new	1_5	N_6
			7	F	6	0_4
			*/
			size_t temp = list[0].next;
			list[temp].element = value;
			list[0].next = list[temp].next;
			list[list[temp].next].prev = 0;
			list[temp].next = head;
			list[temp].prev = NULL;
			list[head].prev = temp;
			head = temp;
			++length;
			return head;
		}
		else {
			/*
			list index=4 head=1 tail=5
			0	F	3	6
			1	a	4	N
			2	c	5	4
			3	F	7	0
			4	b	2	1
			5	d	N	2
			6	F	0	7
			7	F	6	3

			0	F	7_3	6
			2	c	5	3_7
			3_1	new	2_3	4_5
			4	b	3_8	1
			7	F	6	0_4
			*/
			size_t temp = list[0].next;
			list[temp].element = value;
			list[0].next = list[temp].next;
			list[list[temp].next].prev = 0;
			list[temp].prev = ptr;
			list[temp].next = list[ptr].next;
			list[list[temp].next].prev = temp;
			list[ptr].next = temp;
			++length;
		}
		return temp;
	}
	//在指定指针的删除元素
	size_t _removePtr(const size_t& index) {
		if (index == NULL) {
#ifdef DEBUG
			throw std::out_of_range("Index is empty");
#endif //DEBUG
			return index;
		}
		else if (index == head) {
			if (list[head].next = NULL) {//仅头有元素
				head = NULL;
				tail = NULL;
				length = 0;
				return NULL;
			}
			/*
			0	F	1_6	6
			1	F	3_3	0_4	前链表尾 新回收链表头
			3	F	7	1_5	前回收链表头
			4	b	2	N_2	新链表头
			*/
			head = list[head].next;
			list[head].prev = NULL;
			list[index].next = list[0].next;
			list[index].prev = 0;
			list[list[index].next].prev = index;
			list[0].next = index;
			list[index].element = T();
			--length;
			return head;
		}
		else if (index == tail) {
			if (list[tail].prev = NULL) {//仅尾有元素
				head = NULL;
				tail = NULL;
				length = 0;
				return NULL;
			}
			/*
			0	F	5_6	6
			3	F	7	5_5	前回收链表头
			2	b	N_2	4	新链表尾
			5	F	3_3	0_4	前链表尾 新回收链表头
			*/
			tail = list[tail].prev;
			list[tail].next = NULL;
			list[index].next = list[0].next;
			list[index].prev = 0;
			list[list[index].next].prev = index;
			list[0].next = index;
			list[index].element = T();
			return tail;
		}
		else {
			size_t temp = list[index].next;
			/*index=4
			0	F	4_7	6
			1	a	2_2	N	前驱
			4	F	3_4	0_5	回收链表
			2	c	5	1_1	后继
			3	F	7	4_6	前回收链表头
			*/
			list[list[index].next].prev = list[index].prev;
			list[list[index].prev].next = list[index].next;
			list[index].element = T();
			list[index].next = list[0].next;
			list[index].prev = 0;
			list[list[index].next].prev = index;
			list[0].next = index;
			--length;
			return temp;
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
		if (list[0].next == NULL || capacity < 2) {//无多余空间
			_collectUnusedMemory();
			if (list[0].next == NULL || capacity < 2) {
				size_t newCapacity = capacity < 2 ? 2 : capacity * 2;
				_reorderAndResizeCapacity(newCapacity);
			}
		}
		_insertNextPtr(_atPtr(index), value);
		return;
	}
	//在指定位置删除元素
	void remove(size_t index) {
		if (index >= length) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif //DEBUG
			return;
		}
		if (length < capacity / 4 && capacity >= 32) {
			_collectUnusedMemory();
			if (length < capacity / 4 && capacity >= 32) {//二次检测
				_reorderAndResizeCapacity(capacity / 2);
			}
		}
		_removePtr(_atPtr(index));
		return;
	}
	//末尾插入元素
	void push_back(const T& value) {
		if (list[0].next == NULL || capacity < 2) {//无多余空间
			_collectUnusedMemory();
			if (list[0].next == NULL || capacity < 2) {
				size_t newCapacity = capacity < 2 ? 2 : capacity * 2;
				_reorderAndResizeCapacity(newCapacity);
			}
		}
		_insertPrevPtr(tail, value);
	}
	//末尾移除元素
	void pop_back() {
		if (length < capacity / 4 && capacity >= 32) {
			_collectUnusedMemory();
			if (length < capacity / 4 && capacity >= 32) {//二次检测
				_reorderAndResizeCapacity(capacity / 2);
			}
		}
		_removePtr(tail);
	}
	//首位添加元素
	void push_front(const T& value) {
		if (list[0].next == NULL || capacity < 2) {//无多余空间
			_collectUnusedMemory();
			if (list[0].next == NULL || capacity < 2) {
				size_t newCapacity = capacity < 2 ? 2 : capacity * 2;
				_reorderAndResizeCapacity(newCapacity);
			}
		}
		_insertNextPtr(head, value);
	}
	//首位移除元素
	void pop_front() {
		if (length < capacity / 4 && capacity >= 32) {
			_collectUnusedMemory();
			if (length < capacity / 4 && capacity >= 32) {//二次检测
				_reorderAndResizeCapacity(capacity / 2);
			}
		}
		_removePtr(head);
	}
	//对所以元素执行func
	template<typename Function>
	inline void forEachFunction(Function func) {
		for (size_t ptr = head; ptr != NULL; ptr = list[ptr].next) {
			func(list[ptr].element);
		}
	}
	//对所有元素逆向执行func
	template<typename Function>
	void forEachFunctionInReverse(Function func) {
		for (size_t* ptr = tail; ptr != NULL; ptr = list[ptr].prev) {
			func(list[ptr].element);
		}
	}
	//寻找第一个符合cond的元素的索引
	template<typename Condition>
	size_t findFirstPtrByCondition(Condition cond) {
		for (size_t ptr = head; ptr != NULL; ptr = list[ptr].next) {
			if (cond(list[ptr].element))
				return ptr;
		}
		return NULL;
	}
	//寻找最后一个符合cond的元素的索引
	template<typename Condition>
	size_t findLostPtrByCondition(Condition cond) {
		for (size_t* ptr = tail; ptr != NULL; ptr = list[ptr].prev) {
			if (cond(list[ptr].element))
				return ptr;
		}
		return NULL;
	}
	//对符合cond元素执行func
	template<typename Condition, typename Function>
	void forEachFunctionByCondition(Condition cond, Function func) {
		for (size_t ptr = head; ptr != NULL; ptr = list[ptr].next) {
			if (cond(list[ptr].element))
				func(list[ptr].element);
		}
	}
	//逆序对符合cond元素执行func
	template<typename Condition, typename Function>
	void forEachFunctionByConditionInReverse(Condition cond, Function func) {
		for (size_t* ptr = tail; ptr != NULL; ptr = list[ptr].prev) {
			if (cond(list[ptr].element))
				func(list[ptr].element);
		}
	}
	//从index开始正序寻找与value相等元素,返回所在指针,如果没有返回null
	size_t findPtrInOrder(const T& value, size_t index = 0) {//head=0-5-3-2-1-4 index=2 value=4
		for (size_t ptr = _atPtr(startIndex); ptr != NULL; ptr = list[ptr].next) {//i=2 ptr=3;i=3 ptr=2;i=4 ptr=1;i=5 ptr=4;end
			if (list[ptr].element == value) {
				break;
			}
		}
		return ptr;
	}
	//从index开始正序寻找与value相等元素,返回所在索引,如果没有返回length
	size_t findIndexInOrder(const T& value, size_t startIndex = 0) {//head=0-5-3-2-1-4 index=2 value=4
		for (size_t ptr = _atPtr(startIndex); ptr != NULL; ptr = list[ptr].next, ++startIndex) {//i=2 ptr=3;i=3 ptr=2;i=4 ptr=1;i=5 ptr=4;end
			if (list[ptr].element == value) {
				break;
			}
		}
		return startIndex;
	}
	//从index开始正序寻找与value相等元素,返回所在指针,如果没有返回null
	size_t findPtrInReverse(const T& value, size_t index = 0) {//head=0-5-3-2-1-4 index=3 value=3
		for (size_t ptr = _atPtr(startIndex); ptr != NULL; ptr = list[ptr].prev) {//i=3 ptr=2;i=2 ptr=3;end
			if (list[ptr].element == value) {
				break;
			}
		}
		return ptr;
	}
	//从index开始倒序寻找与value相等元素,返回所在索引,如果没有返回length
	size_t findIndexInReverse(const T& value, size_t startIndex = 0) {//head=0-5-3-2-1-4 index=3 value=3
		for (size_t ptr = _atPtr(startIndex); ptr != NULL; ptr = list[ptr].prev, --startIndex) {//i=3 ptr=2;i=2 ptr=3;end
			if (list[ptr].element == value) {
				break;
			}
		}//i=3 ptr=3;i=2 ptr=2;i=1 ptr=1;end
		return startIndex;
	}
};
