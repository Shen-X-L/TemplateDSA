#pragma once
#include "myHead.h"

template<typename T>
class LinkedList {
protected:
	template<typename T>
	struct ListNode {//����ṹ
		using Node = ListNode<T>;
		T element;
		Node* next;
		ListNode() :element() {};
		ListNode(const T& in, Node* ptr = nullptr) :element(in), next(ptr) {};
		ListNode(T&& in, Node* ptr = nullptr) :element(std::move(in)), next(ptr) {};
	};
	using Node = ListNode<T>;
	Node* head;  //����ͷָ��  
	size_t length;//����
public:
	//���캯��
	LinkedList(size_t initialSize = 0) 
		:head(nullptr), length(initialSize) {
		if (!length) {
			return;
		}//size=length=3 head=0->1->2
		head = new Node(T());//head=0
		Node* prev = head;//ǰ��
		for (size_t i = 1; i < length; ++i) {//i=1 next=1 prev=0->1 prev=1 ++i
			prev->next = new Node(T());//i=2 next=2 prev=1->2 prev=2 ++i
			prev = prev->next;//i=3 end
		}
	}
	//����size��inԪ��
	LinkedList(size_t initialSize, const T& in) 
		:head(nullptr), length(initialSize) {
		if (!length) {
			return;
		}//size=length=3 head=0->1->2
		head = new Node(in);//head=0
		Node* prev = head;//ǰ��
		for (size_t i = 1; i < length; ++i) {//i=1 next=1 prev=0->1 prev=1 ++i
			prev->next = new Node(T(in));//i=2 next=2 prev=1->2 prev=2 ++i
			prev = prev->next;//i=3 end
		}
	}
	//�б��ʼ��
	LinkedList(std::initializer_list<T> in) 
		:head(nullptr), length(in.size()) {
		if (!length) {
			return;
		}
		const T* it = in.begin();
		head = new Node(*it);
		Node* prev = head;//ǰ��
		for (++it; it != in.end(); ++it) {
			prev->next = new Node(*it);
			prev = prev->next;
		}
	}
	//��������
	LinkedList(const LinkedList& other) 
		:head(nullptr), length(other.length) {
		if (other.head == nullptr) {
			length = 0;
			return;
		}
		head = new Node(other.head->element);
		Node* prev = head;//ǰ��
		for (Node* tempother = other.head->next; tempother != nullptr; tempother = tempother->next) {
			prev->next = new Node(tempother->element);
			prev = prev->next;
		}
	}
	//�ƶ�����
	LinkedList(LinkedList&& other)noexcept 
		:head(other.head), length(other.length) {
		other.head = nullptr;
		other.length = 0;
	}
	//�ƶ�����
	~LinkedList() {
		clear();
	}
	//�������
	inline void clear() {
		_clearPtr(head);
	}
	//���ptr�Լ�֮�����������
	void _clearPtr(Node* ptr) {
		Node* next = nullptr;//���
		for (; ptr != nullptr;) {//head=0->1->2->3 ptr=1
			next = ptr->next;//next=2 delete 1 ptr=next=2
			delete ptr;//next=3 delete 2  ptr=next=3
			ptr = next;//next=null delete 3 ptr=null end
		}
		_alignLength();
	}
	//������ֵ
	LinkedList& operator=(const LinkedList& other) {
		if (this != &other){
			//�������
			clear();
			//��������
			if (other.head == nullptr) {
				length = 0;
				return *this;
			}
			length = other.length;
			head = new Node(other.head->element);
			Node* prev = head;//ǰ��
			for (Node* tempother = other.head->next; tempother != nullptr; tempother = tempother->next) {
				prev->next = new Node(tempother->element);
				prev = prev->next;
			}
		}
		return *this;
	}
	//�ƶ���ֵ
	LinkedList& operator=(LinkedList&& other) noexcept {
		if (this != &other) {
			//�������
			clear();
			length = other.length;
			head = other.head;
			other.length = 0;
			other.head = nullptr;
		}
		return *this;
	}
	//У׼������ 
	size_t _alignLength() {//head=0->1->2
		length = 0;
		for (Node* ptr = head; ptr != nullptr; ++length) { ptr = ptr->next; }//ptr=0 i=0;ptr=1 i=1;ptr=2 i=2;ptr=null i=3;end
		return length;
	}
	//����������
	inline size_t size() const {
		return length;
	}
	//�������Ϊ�� ����TRUE
	inline bool empty() const {
		return head == nullptr;
	}
	//��ת����
	void reverse() {
		if (head == nullptr || head->next == nullptr) {
			return; // �������ֻ��һ���ڵ㣬���跴ת
		}
		Node* prev = nullptr;
		Node* curr = head;
		Node* nextNode = nullptr;
		while (curr != nullptr) {
			nextNode = curr->next; // ������һ���ڵ�
			curr->next = prev; // ��ǰ�ڵ�ָ��ǰһ���ڵ㣬ʵ�ַ�ת
			prev = curr; // ����ǰһ���ڵ�Ϊ��ǰ�ڵ�
			curr = nextNode; // ���µ�ǰ�ڵ�Ϊ��һ���ڵ�
		}
		head = prev; // ����ͷָ��Ϊԭ�������β�ڵ�
	}
	//����ָ��ָ��Ԫ��
	inline static T& getElem(Node* ptr) {
		return ptr->element;
	}
	inline static const T& getElem(Node* ptr) const{
		return ptr->element;
	}
	//����ָ����
	inline static Node* getNext(Node* ptr) {
		return ptr->next;
	}
	inline static const Node* getNext(Node* ptr) const {
		return ptr->next;
	}
	//���ʵ�index����ָ��
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
	//����Ԫ������
	T& operator[](size_t index) {
		return  _atPtr(index)->element;
	}
	//����Ԫ������
	const T& operator[](size_t index) const {
		return  _atPtr(index)->element;
	}
	//��ָ��ָ��ĺ�̲���Ԫ��
	//ָ��ָ����ܲ��Ǳ�����ָ��,С��ʹ��
	//�����ڲ�����,��û�ж�length�޸�
	static Node* _insertNextPtr(const Node*& ptr, const T& value) {
		if (ptr != nullptr) {
			Node* next = new Node(value, ptr->next);
			ptr->next = next;
			return next;
		}
		return nullptr;
	}
	//��ָ��ָ��ĺ��ɾ��Ԫ��
	//ָ��ָ����ܲ��Ǳ�����ָ��,С��ʹ��
	static Node* _removeNextPtr(const Node*& ptr) {
		if (ptr != nullptr && ptr->next != nullptr) {
			Node* next = ptr->next;
			ptr->next = next->next;
			delete next;
			return ptr;
		}
		return nullptr;
	}
	//��ָ��λ�ò���Ԫ��
	//��0-1-2 ����(3 new) 0-1-2-new
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
				throw std::out_of_range("Invalid index");// ���� prev Ϊ nullptr �����
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
	//ɾ��ָ������λ�õ�Ԫ��  
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
				throw std::out_of_range("Invalid index");// ���� prev Ϊ nullptr �����
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
	//ĩβ����Ԫ��
	void push_back(const T& value) {
		Node* prev;
		for (prev = head; prev->next != nullptr; prev = prev->next) {};
		prev->next = new Node(value);
		++length;
	}
	//ĩβ�Ƴ�Ԫ��
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
	//��λ���Ԫ��
	void push_front(const T& value) {
		Node* ptr = head;
		head = new Node(value);
		head->next = ptr;
		++length;
	}
	//��λ�Ƴ�Ԫ��
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
	//������Ԫ��ִ��func
	template<typename Function>
	inline void forEachFunction(Function func) {
		for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) {
			func(ptr->element);
		}
	}
	//Ѱ�ҵ�һ������cond��Ԫ�ص�����
	template<typename Condition>
	inline Node* findFirstByCondition(Condition cond) {
		for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) {
			if (cond(ptr->element))
				return ptr;
		}
		return nullptr;
	}
	//�Է���condԪ��ִ��func
	template<typename Condition, typename Function>
	inline void forEachFunctionByCondition(Condition cond, Function func) {
		for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) {
			if (cond(ptr->element))
				func(ptr->element);
		}
	}
	//��index��ʼѰ����value���Ԫ��,��������ָ��,���û�з���nullptr
	inline Node* findPtrInOrder(const T& value, size_t index = 0) {//head=0-1-2-3-4-5 index=2 value=4
		for (Node* ptr = _atPtr(index); ptr != nullptr; ptr = ptr->next;) {//i=2 ptr=2;i=3 ptr=3;i=4 ptr=4;end
			if (ptr->element == value) {
				return ptr;
			}
		}
		return nullptr;
	}
	//��index��ʼѰ����value���Ԫ��,������������,���û�з���length
	inline size_t findIndexInOrder(const T& value, size_t startIndex = 0) {//head=0-1-2-3-4-5 index=2 value=4
		for (Node* ptr = _atPtr(startIndex); ptr != nullptr && startIndex < length; ptr = ptr->next, ++startIndex;) {//i=2 ptr=2;i=3 ptr=3;i=4 ptr=4;end
			if (ptr->element == value) {
				return startIndex;
			}
		}
		return length;
	}
};
