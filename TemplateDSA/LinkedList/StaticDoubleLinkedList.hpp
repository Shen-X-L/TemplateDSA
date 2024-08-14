#pragma once
#include "myHead.h"

template<typename T>
class StaticDoubleLinkedList {//��̬��
	/*
	id	elem	next	prev
	0	Nul		5		4		��������
	1	a		3		Nul		ʹ������
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
	size_t length;//Ԫ�ظ���
	size_t capacity;//�����ڴ泤�� 
	size_t head;//��Ԫ��λ�� ���Ϊ0����ձ�
	size_t tail;//βԪ��λ��
	Node* list;
public:
	//���캯��
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
	//����size��inԪ�� 
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
	//�б��ʼ��
	//ͬ��
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
	//��������
	StaticDoubleLinkedList(const StaticDoubleLinkedList& other)
		:length(other.length), capacity(other.capacity), head(other.head), tail(other.tail), list(new Node[capacity]) {
		for (size_t i = 0; i < capacity; ++i) {
			list[i] = other.list[i];
		}
	}
	//�ƶ�����
	StaticDoubleLinkedList(StaticDoubleLinkedList&& other) noexcept
		:length(other.length), capacity(other.capacity), head(other.head), tail(other.tail), list(other.list) {
		other.length = 0;
		other.capacity = 0;
		other.head = 0;
		other.tail = 0;
		other.list = nullptr;
	}
	//��������
	~StaticDoubleLinkedList() {
		delete[] list;
	}
	//������ֵ
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
	//�ƶ���ֵ
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
	//У׼������ 
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
	//������
	//У׼������ 
	//����Ƿ����ѭ������
	//����������Ϊ��׼ ���ǰ�������Ƿ���ȷ
	//�����޹�������
	//set����������������Ϊ��׼
	int _collectUnusedMemory(int set) {
		int error;//������� ���ش������� �Ժ���д
		size_t templength = 0;
		bool* isNode = new bool[capacity]();//��Ȼ����ж�bool[]����ѹ���ڴ� Ŀǰ�ȸ���
		if (set == 1) {//����
			size_t i = head;
			for (size_t prev = NULL; i != NULL; i = list[i].next) {//���i==NULL��������
				if (list[i].prev != prev) {
					error |= 0x1;//ǰ��������
					list[i].prev = prev;
				}
				prev = i;
				++templength;
				isNode[i] = 1;
				if (isNode[list[i].next] == 1) {//isNude[list[i].next]==1�����ѭ�� ���� 
					list[i].next = NULL;//�ض�ѭ����
					error |= 0x2;
					break;
				}
			}
			if (i != tail) {//β��δ����
				tail = i;
				error |= 0x4;
			}
		}
		else {
			size_t i = tail;
			for (size_t next = NULL; i != NULL; i = list[i].prev) {//���i==NULL��������
				if (list[i].next != next) {
					error |= 0x10;//��̲�����
					list[i].next = next;
				}
				next = i;
				++templength;
				isNode[i] = 1;
				if (isNode[list[i].prev] == 1) {//isNude[list[i].next]==1�����ѭ�� ���� 
					list[i].prev = NULL;//�ض�ѭ����
					error |= 0x20;
					break;
				}
			}
			if (i != head) {//ͷ��δ����
				head = i;
				error |= 0x40;
			}
		}
		if (length != templength) {//����δ����
			length = templength;
			error |= 0x100;
		}
		/*
		Ӧ����2 3 5
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
	//���������� 
	//�ڴ��ط���
	//����У׼ ��ѭ������
	/*
	length = 3 head = 1
	this			temp
	ind	ele	nex	pre	ind	ele	nex	pre	(����move���next��Ա��֪����������,Ĭ��Ϊ����,ʵ�ʲ�Ӱ���߼�)
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
	//����������
	size_t size() const {
		return length;
	}
	//�������Ϊ�� ����TRUE
	bool empty() const {
		if (head == NULL && tail == NULL) {
			return true;
		}
		if (head == NULL || tail == NULL) {//��һͷΪ��,�������
			_collectUnusedMemory(1);
			return empty;
		}
		else
			return false;
	}
	//����ָ��ָ��Ԫ��
	T& getElem(size_t ptr) {
		return list[ptr].element;
	}
	const T& getElem(size_t ptr) const {
		return list[ptr].element;
	}
	//����ָ����
	size_t getNext(size_t ptr) {
		return list[ptr].next;
	}
	//����ָ��ǰ��
	size_t getPrev(size_t ptr) {
		return list[ptr].prev;
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
	//���ʵ�index����ָ��
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
	//����Ԫ������
	T& operator[](size_t index) {
		return  list[_atPtr(index)].element;
	}
	//����Ԫ������
	const T& operator[](size_t index) const {
		return  list[_atPtr(index)].element;
	}
	//��ָ��λ�õ�ǰ������Ԫ��
	size_t _insertPrevPtr(const size_t& ptr, const T& value) {
		if (list[0].next == NULL || capacity < 2) {//�޶���ռ�
			size_t newCapacity = capacity < 2 ? 2 : capacity * 2;
			Node* newList = new Node[newCapacity];
			for (size_t i = 0; i < capacity; ++i) { 
				newList[i] = std::move(list[i]); 
			}
			delete[] list;
			list = newList;
			/*
			cap=5 Newcap=8	ind	nex	pre	 �м������޸�
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
		if (ptr == head) {//��ͷ������
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
		else if(ptr == NULL){//��β������
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
	//��ָ��λ�õĺ�̲���Ԫ��
	size_t _insertNextPtr(const size_t& ptr, const T& value) {
		if (list[0].next == NULL || capacity < 2) {//�޶���ռ�
			size_t newCapacity = capacity < 2 ? 2 : capacity * 2;
			Node* newList = new Node[newCapacity];
			for (size_t i = 0; i < capacity; ++i) {
				newList[i] = std::move(list[i]);
			}
			delete[] list;
			list = newList;
			/*
			cap=5 Newcap=8	ind	nex	pre	 �м������޸�
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
		if (ptr == tail) {//��β������
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
		else if (ptr == NULL) {//��ͷ������
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
	//��ָ��ָ���ɾ��Ԫ��
	size_t _removePtr(const size_t& index) {
		if (index == NULL) {
#ifdef DEBUG
			throw std::out_of_range("Index is empty");
#endif //DEBUG
			return index;
		}
		else if (index == head) {
			if (list[head].next = NULL) {//��ͷ��Ԫ��
				head = NULL;
				tail = NULL;
				length = 0;
				return NULL;
			}
			/*
			0	F	1_6	6
			1	F	3_3	0_4	ǰ����β �»�������ͷ
			3	F	7	1_5	ǰ��������ͷ
			4	b	2	N_2	������ͷ
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
			if (list[tail].prev = NULL) {//��β��Ԫ��
				head = NULL;
				tail = NULL;
				length = 0;
				return NULL;
			}
			/*
			0	F	5_6	6
			3	F	7	5_5	ǰ��������ͷ
			2	b	N_2	4	������β
			5	F	3_3	0_4	ǰ����β �»�������ͷ
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
			1	a	2_2	N	ǰ��
			4	F	3_4	0_5	��������
			2	c	5	1_1	���
			3	F	7	4_6	ǰ��������ͷ
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
		_insertNextPtr(_atPtr(index), value);
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
		_removePtr(_atPtr(index));
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
		_insertPrevPtr(tail, value);
	}
	//ĩβ�Ƴ�Ԫ��
	void pop_back() {
		if (length < capacity / 4 && capacity >= 32) {
			_collectUnusedMemory();
			if (length < capacity / 4 && capacity >= 32) {//���μ��
				_reorderAndResizeCapacity(capacity / 2);
			}
		}
		_removePtr(tail);
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
		_insertNextPtr(head, value);
	}
	//��λ�Ƴ�Ԫ��
	void pop_front() {
		if (length < capacity / 4 && capacity >= 32) {
			_collectUnusedMemory();
			if (length < capacity / 4 && capacity >= 32) {//���μ��
				_reorderAndResizeCapacity(capacity / 2);
			}
		}
		_removePtr(head);
	}
	//������Ԫ��ִ��func
	template<typename Function>
	inline void forEachFunction(Function func) {
		for (size_t ptr = head; ptr != NULL; ptr = list[ptr].next) {
			func(list[ptr].element);
		}
	}
	//������Ԫ������ִ��func
	template<typename Function>
	void forEachFunctionInReverse(Function func) {
		for (size_t* ptr = tail; ptr != NULL; ptr = list[ptr].prev) {
			func(list[ptr].element);
		}
	}
	//Ѱ�ҵ�һ������cond��Ԫ�ص�����
	template<typename Condition>
	size_t findFirstPtrByCondition(Condition cond) {
		for (size_t ptr = head; ptr != NULL; ptr = list[ptr].next) {
			if (cond(list[ptr].element))
				return ptr;
		}
		return NULL;
	}
	//Ѱ�����һ������cond��Ԫ�ص�����
	template<typename Condition>
	size_t findLostPtrByCondition(Condition cond) {
		for (size_t* ptr = tail; ptr != NULL; ptr = list[ptr].prev) {
			if (cond(list[ptr].element))
				return ptr;
		}
		return NULL;
	}
	//�Է���condԪ��ִ��func
	template<typename Condition, typename Function>
	void forEachFunctionByCondition(Condition cond, Function func) {
		for (size_t ptr = head; ptr != NULL; ptr = list[ptr].next) {
			if (cond(list[ptr].element))
				func(list[ptr].element);
		}
	}
	//����Է���condԪ��ִ��func
	template<typename Condition, typename Function>
	void forEachFunctionByConditionInReverse(Condition cond, Function func) {
		for (size_t* ptr = tail; ptr != NULL; ptr = list[ptr].prev) {
			if (cond(list[ptr].element))
				func(list[ptr].element);
		}
	}
	//��index��ʼ����Ѱ����value���Ԫ��,��������ָ��,���û�з���null
	size_t findPtrInOrder(const T& value, size_t index = 0) {//head=0-5-3-2-1-4 index=2 value=4
		for (size_t ptr = _atPtr(startIndex); ptr != NULL; ptr = list[ptr].next) {//i=2 ptr=3;i=3 ptr=2;i=4 ptr=1;i=5 ptr=4;end
			if (list[ptr].element == value) {
				break;
			}
		}
		return ptr;
	}
	//��index��ʼ����Ѱ����value���Ԫ��,������������,���û�з���length
	size_t findIndexInOrder(const T& value, size_t startIndex = 0) {//head=0-5-3-2-1-4 index=2 value=4
		for (size_t ptr = _atPtr(startIndex); ptr != NULL; ptr = list[ptr].next, ++startIndex) {//i=2 ptr=3;i=3 ptr=2;i=4 ptr=1;i=5 ptr=4;end
			if (list[ptr].element == value) {
				break;
			}
		}
		return startIndex;
	}
	//��index��ʼ����Ѱ����value���Ԫ��,��������ָ��,���û�з���null
	size_t findPtrInReverse(const T& value, size_t index = 0) {//head=0-5-3-2-1-4 index=3 value=3
		for (size_t ptr = _atPtr(startIndex); ptr != NULL; ptr = list[ptr].prev) {//i=3 ptr=2;i=2 ptr=3;end
			if (list[ptr].element == value) {
				break;
			}
		}
		return ptr;
	}
	//��index��ʼ����Ѱ����value���Ԫ��,������������,���û�з���length
	size_t findIndexInReverse(const T& value, size_t startIndex = 0) {//head=0-5-3-2-1-4 index=3 value=3
		for (size_t ptr = _atPtr(startIndex); ptr != NULL; ptr = list[ptr].prev, --startIndex) {//i=3 ptr=2;i=2 ptr=3;end
			if (list[ptr].element == value) {
				break;
			}
		}//i=3 ptr=3;i=2 ptr=2;i=1 ptr=1;end
		return startIndex;
	}
};
