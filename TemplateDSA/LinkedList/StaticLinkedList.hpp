#pragma once
#include "myHead.h"

template<typename T>
class StaticLinkedList{//��̬����
	/*
	0	null	5	��������
	1	a		3	ʹ������	
	2	b		null
	3	c		2
	4	null	null
	5	null	4
	1-3-2-null	0-5-4-null
	*/
protected:
	template<typename T>
	struct ListNode {
		using Node = ListNode<T>;
		T element;
		size_t next;
		ListNode() :next(NULL) {};
		ListNode(const T& in, size_t index = NULL) :element(in), next(index) {};
		ListNode(T&& in, size_t index = NULL) :element(std::move(in)), next(index) {};
		Node& operator=(const Node& other) {
			if (this != &other) {
				element = other.element;
				next = other.next;
			}
			return *this;
		}
		Node& operator=(Node&& other) {
			if (this != &other) {
			element = std::move(other.element);
			next = other.next;
			}
			return *this;
		}
	};
	using Node = ListNode<T>;
	size_t length;//Ԫ�ظ���
	size_t capacity;//�����ڴ泤�� 
	size_t head;//��Ԫ��λ�� ���Ϊ0���������
	Node* list;
public:
	//���캯��
	/* 
	0	null	1
	1	null	2
	2	null	3
	3	null	null;
	*/
	StaticLinkedList(size_t initialSize = 0)
		:length(0), capacity(initialSize ? initialSize + 1 : 0), head(0), list(capacity ? new T[capacity] : nullptr) {
		if (list) {
			for (size_t i = 0; i < capacity; ++i) {//
				list[i].next = i + 1;
			}
			list[capacity - 1].next = NULL;//3 null null
		}
	}
	//����size��inԪ��
	/*
	i=1
	0	null	null
	1	a		2
	i=2
	0	null	null
	1	a		2
	2	b		3
	i=3
	0	null	null
	1	a		2
	2	b		3
	3	c		4
	end
	3	c		null
	*/
	StaticLinkedList(size_t initialSize, const T& in) 
		:length(initialSize), capacity(initialSize ? initialSize + 1 : 0), head(0), list(capacity ? new T[capacity] : nullptr) {
		if (list) {
			for (size_t i = 1; i <= length; ++i) {
				list[i].element = in;
				list[i].next = i + 1;
			}
			list[length].next = NULL;
		}
	}
	//�б��ʼ��
	StaticLinkedList(std::initializer_list<T> in) 
		:length(in.size()), capacity(in.size() ? in.size() + 1 : 0), head(0), list(capacity ? new T[capacity] : nullptr) {
		if (list) {
			size_t i = 1;
			for (const T* it = in.begin(); it != in.end(); ++it, ++i) {
				list[i].element = *it;
				list[i - 1].next = i;
			}
			list[0].next = NULL;
		}

	}
	//��������
	StaticLinkedList(const StaticLinkedList& other) 
		:length(other.length), capacity(other.capacity), head(other.head),list(new Node[capacity]) {
		for (size_t i = 0; i < capacity; ++i) {
			list[i] = other.list[i];
		}
	}
	//�ƶ�����
	StaticLinkedList(StaticLinkedList&& other) noexcept 
		:length(other.length), capacity(other.capacity), head(other.head), list(other.list) {
		other.list = nullptr;
		other.length = 0;
		other.capacity = 0;
		other.head = 0;
	}
	//��������
	~StaticLinkedList() {
		delete[] list;
	}
	//������ֵ
	StaticLinkedList<T>& operator=(const StaticLinkedList& other) {
		if (this != &other) {
			delete[] list;
			length = other.length;
			capacity = other.capacity;
			head = other.head;
			list = new Node[capacity];
			for (size_t i = 0; i < capacity; ++i) {
				list[i] = other.list[i];
			}
		}
		return *this;
	}
	//�ƶ���ֵ
	StaticLinkedList<T>& operator=(StaticLinkedList&& other) noexcept {
		if (this != &other) {
			delete[] list;
			list = other.list;
			length = other.length;
			capacity = other.capacity;
			head = other.head;
			other.list = nullptr;
			other.length = 0;
			other.capacity = 0;
			other.head = 0;
		}
		return *this;
	}
	//У׼������ 
	/*  head=1
	0 3 length=3
	1 4 length=0
	2 N length=2
	3 N
	4 2 length=1
	*/
	inline size_t _alignLength() {
		length = 0;
		for (size_t ptr = head; ptr != NULL; ++length) {ptr = list[ptr].next;}
		return length;
	}
	//������
	//У׼������ 
	//�����޹�������
	int _collectUnusedMemory() {
		int error;//������� ���ش������� �Ժ���д
		size_t templength = 0;
		bool* isNode = new bool[capacity]();//��Ȼ����ж�bool[]����ѹ���ڴ� Ŀǰ�ȸ���
		for (size_t i = head; i != NULL; i = list[i].next) {//���i==NULL��������
			++templength;
			isNode[i] = true;
			if (isNode[list[i].next]) {//isNude[list[i].next]==1�����ѭ�� ���� 
				list[i].next = NULL;//�ض�ѭ����
				error |= 0b10;
				break;
			}
		}
		if (length != templength) {
			length = templength;
			error |= 0b1;
		}
		/*
		Ӧ����2 3 5
		i=2	coll=0
		0	2
		2	X
		i=3	coll=2
		0	2
		2	3
		3	X
		i=5	coll=3
		0	2
		2	3
		3	5
		5	X
		coll=5
		5	0
		*/
		size_t collect = 0;
		for (size_t i = 1; i < capacity; ++i) {
			if (!isNode[i]) {
				list[collect].next = i;
				collect = i;
			}
		}
		list[collect].next = NULL;
		delete[] isNode;
		return error;
	}
	//����������
	//�ڴ��ط���
	//Ĭ����ѭ������ ������У׼
	/*
	length = 3 head = 1 this = 0 3  1a 4  2c N  3 5  4b 2  5 N; i = 1 j = 1 next = 0; temp = 0 N  1 N  2 N;
	next=4;temp=0  N	1a 2	2  N	3  N	4  N	5  N;i=4;j=2;(����move���next��Ա��֪����������,Ĭ��Ϊ����,ʵ�ʲ�Ӱ���߼�)
	next=2;temp=0  N	1a 2	2b 3	3  N	4  N	5  N;i=2;j=3;	
	next=N;temp=0  N	1a 2	2b 3	3c 4	4  N	5  N;i=N;j=4;end 
	temp=0  N	1a 2	2b 3	3c N	4  5	5  6;j=6;
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
		for (size_t i = head, next = 0; i != NULL || j <= length; ++j) {
			next = list[i].next;
			newList[j] = std::move(list[i]);
			newList[j].next = j + 1;
			i = next;
		}
		newList[length].next = NULL;
		if (length + 1 != newCapacity) {
			newList[0].next = j;
			for (; j < newCapacity; ++j) {
				newList[j].next = j + 1;
			}
			newList[newCapacity - 1].next = NULL;
		}
		delete[] list;
		list = newList;
		capacity = newCapacity;
		head = 1;
		return;
	}
	//����������
	inline size_t size() const {
		return length;
	}
	//�������Ϊ�� ����TRUE
	inline bool empty() const {
		return head == NULL;
	}
	//��ת����
	void reverse() {
		if (head == NULL || list[head].next == NULL) {
			return; // �������ֻ��һ���ڵ㣬���跴ת
		}
		size_t prev = NULL;
		size_t curr = head;
		size_t nextNode = NULL;
		while (curr != NULL) {
			nextNode = list[curr].next; // ������һ���ڵ�
			list[NULL].next = prev; // ��ǰ�ڵ�ָ��ǰһ���ڵ㣬ʵ�ַ�ת
			prev = curr; // ����ǰһ���ڵ�Ϊ��ǰ�ڵ�
			curr = nextNode; // ���µ�ǰ�ڵ�Ϊ��һ���ڵ�
		}
		head = prev; // ����ͷָ��Ϊԭ�������β�ڵ�
	}
	//���ʵ�index����ָ��
	/*
	head=1 length=4 index=2
	0 3
	1 4 ptr i=0
	2 5 ptr i=2
	3 N
	4 2 ptr i=1
	5 N
	*/
	//����ָ��ָ��Ԫ��
	inline T& getElem(size_t ptr) {
		return list[ptr].element;
	}
	inline const T& getElem(size_t ptr) const {
		return list[ptr].element;
	}
	//����ָ����
	inline size_t getNext(size_t ptr) {
		return list[ptr].next;
	}
	//���ʵ�index����ָ��
	size_t _atPtr(const size_t& index) {
		if (index >= length) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif
			return NULL;
		}
		size_t ptr = head;
		for (size_t i = 0; i < index && ptr != NULL; ++i) { ptr = list[ptr].next; }
		return ptr;
	}
	//���ʵ�index����ָ��
	const size_t _atPtr(const size_t& index) const {
		if (index >= length) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif
			return NULL;
		}
		size_t ptr = head;
		for (size_t i = 0; i < index && ptr != NULL; ++i) { ptr = list[ptr].next; }
		return ptr;
	}
	//����Ԫ������
	inline T& operator[](size_t index) {
		return  list[_atPtr(index)].element;
	}
	//����Ԫ������
	inline const T& operator[](size_t index) const {
		return  list[_atPtr(index)].element;
	}
	//��ָ��λ�õĺ�̲���Ԫ��
	//���ΪNULL������ͷǰ����,���±�ͷ
	size_t _insertNextPtr(const size_t& index, const T& value) {
		if (index == NULL) {
			size_t temp = list[0].next;
			list[temp].element = value;
			list[0].next = list[temp].next;
			list[temp].next = head;
			head = temp;
			++length;
			return head;
		}
		/*
		0 F 3	ptr=4
		1 a 4	temp=3 v 6
		2 c 5	0 F 6
		3 F 6	3 v 2
		4 b 2	4 b 3
		5 d N
		6 F N
		*/
		size_t temp = list[0].next;
		list[temp].element = value;
		list[0].next = list[temp].next;
		list[temp].next = list[index].next;
		list[index].next = temp;
		++length;
		return temp;
	}
	//��ָ��ָ��ĺ��ɾ��Ԫ��
	//���ΪNULLɾ����ͷ
	size_t _removeNextPtr(const size_t& index) {
		if (index == NULL && head != NULL) {//�������ͷ
			size_t temp = head;
			head = list[head].next;
			list[temp].next = list[0].next;
			list[0].next = temp;
			--length;
			return index;
		}
		if (list[index].next != NULL) {
			size_t temp = list[index].next;
			list[index].next = list[temp].next;
			list[temp].next = list[0].next;
			list[0].next = temp;
			--length;
			return index;
		}
		return index;
	}
	//��ָ��λ�ò���Ԫ��
	void insert(size_t index, const T& value) {
		if (index > length) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif //DEBUG
			return;
		}
		if (list[0].next == NULL || capacity < 2) {//�޶���ռ�
			_collectUnusedMemory();
			if (list[0].next == NULL || capacity < 2) {
				size_t newCapacity = capacity < 2 ? 2 : capacity * 2;
				_reorderAndResizeCapacity(newCapacity);
			}
		}
		index ? _insertNextPtr(_atPtr(index - 1), value) : _insertNextPtr(_atPtr(NULL), value);
		return;
	}
	//��ָ��λ��ɾ��Ԫ��
	void remove(size_t index) {
		if (index >= length) {
#ifdef DEBUG
			throw std::out_of_range("Index out of range");
#endif //DEBUG
			return;
		}
		if (length < capacity / 4 && capacity >= 32) {
			_collectUnusedMemory();
			if (length < capacity / 4 && capacity >= 32) {//���μ��
				_reorderAndResizeCapacity(capacity / 2);
			}
		}
		index ? _removeNextPtr(_atPtr(index - 1)) : _removeNextPtr(_atPtr(NULL));
		return;
	}
	//ĩβ����Ԫ��
	void push_back(const T& value) {
		if (list[0].next == NULL || capacity < 2) {//�޶���ռ�
			_collectUnusedMemory();
			if (list[0].next == NULL || capacity < 2) {
				size_t newCapacity = capacity < 2 ? 2 : capacity * 2;
				_reorderAndResizeCapacity(newCapacity);
			}
		}
		size_t prev;
		for (prev = head; list[prev].next != NULL; prev = list[prev].next) {};
		_insertNextPtr(prev, value);
	}
	//ĩβ�Ƴ�Ԫ��
	void pop_back() {
		if (length < capacity / 4 && capacity >= 32) {
			_collectUnusedMemory();
			if (length < capacity / 4 && capacity >= 32) {//���μ��
				_reorderAndResizeCapacity(capacity / 2);
			}
		}
		if (head == NULL) {
			length = 0;
			return;
		}
		if (list[head].next == NULL) {
			list[head].element = T();
			list[head].next = list[0].next;
			list[0].next = head;
			head=NULL;
			length = 0;
			return;
		}
		size_t prev;
		for (prev = head; list[list[prev].next].next != NULL; prev = list[prev].next) {};
		_removeNextPtr(prev);
	}
	//��λ���Ԫ��
	void push_front(const T& value) {
		if (list[0].next == NULL || capacity < 2) {//�޶���ռ�
			_collectUnusedMemory();
			if (list[0].next == NULL || capacity < 2) {
				size_t newCapacity = capacity < 2 ? 2 : capacity * 2;
				_reorderAndResizeCapacity(newCapacity);
			}
		}
		_insertNextPtr(_atPtr(NULL), value);
	}
	//��λ�Ƴ�Ԫ��
	void pop_front() {
		if (length < capacity / 4 && capacity >= 32) {
			_collectUnusedMemory();
			if (length < capacity / 4 && capacity >= 32) {//���μ��
				_reorderAndResizeCapacity(capacity / 2);
			}
		}
		_removeNextPtr(_atPtr(NULL));
	}
	//������Ԫ��ִ��func
	template<typename Function>
	inline void forEachFunction(Function func) {
		for (size_t ptr = head; ptr != NULL; ptr = list[ptr].next) {
			func(list[ptr].element);
		}
	}
	//Ѱ�ҵ�һ������cond��Ԫ�ص�����
	template<typename Condition>
	inline size_t findFirstByCondition(Condition cond) {
		for (size_t ptr = head; ptr != NULL; ptr = list[ptr].next) {
			if (cond(list[ptr].element))
				return ptr;
		}
		return length;
	}
	//�Է���condԪ��ִ��func
	template<typename Condition, typename Function>
	inline void forEachFunctionByCondition(Condition cond, Function func) {
		for (size_t ptr = head; ptr != NULL; ptr = list[ptr].next) {
			if (cond(list[ptr].element))
				func(list[ptr].element);
		}
	}
	//��index��ʼѰ����value���Ԫ��,��������ָ��,���û�з���NULL
	inline size_t findPtrInOrder(const T& value, size_t index = 0) {//head=0-1-2-3-4-5 index=2 value=4
		for (size_t ptr = _atPtr(index); ptr != NULL; ptr = list[ptr].next;) {
			if (list[ptr].element == value) {//i=2 ptr=2;i=3 ptr=3;i=4 ptr=4;end
				return ptr;
			}
		}
		return NULL;
	}
	//��index��ʼѰ����value���Ԫ��,������������,���û�з���length
	inline size_t findIndexInOrder(const T& value, size_t startIndex = 0) {//head=0-1-2-3-4-5 index=2 value=4
		size_t index = startIndex;
		for (Node* ptr = _atPtr(startIndex); ptr != NULL && index < length; ptr = list[ptr].next, ++index;) {//i=2 ptr=2;i=3 ptr=3;i=4 ptr=4;end
			if (list[ptr].element == value) {
				return index;
			}
		}
		return length;
	}
};

