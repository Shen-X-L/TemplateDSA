#pragma once
#include "myHead.h"

template<typename T>
class LinkQueue {
protected:
	template<typename T>
	struct ListNode {//����ṹ
		using Node = ListNode<T>;
		T element;
		Node* next = nullptr;
		ListNode() :element() {};
		ListNode(const T& in, Node* ptr = nullptr) :element(in), next(ptr) {};
		ListNode(T&& in, Node* ptr = nullptr) :element(std::move(in)), next(ptr) {};
	};
	using Node = ListNode<T>;
	//front 0->1->2->3 rear
	Node* front;//����
	Node* rear;//��β
public:
	//���캯��
	LinkQueue() {};
	//����size��inԪ��
	LinkQueue(size_t initialSize, const T& in) 
		:front(nullptr), rear(nullptr) {
		if (!initialSize) {
			return;
		}//front=0->1->2=rear
		front = new Node(in);//top=0
		rear = front;
		Node* prev = front;
		for (size_t i = 1; i < initialSize; ++i) {//i=1 rear=1 prev=0->1 prev=1 ++i
			rear = new Node(in);//i=2 rear=2 prev=1->2 prev=2 ++i
			prev->next = rear;
			prev = rear;
		}
	}
	//�б��ʼ��
	LinkQueue(std::initializer_list<T> in) 
		:front(nullptr), rear(nullptr) {
		if (!in.size()) {
			return;
		}//top=2->1->0
		const T* it = in.begin();
		front = new Node(*it);//top=0
		rear = front;
		Node* prev = front;
		for (++it; it != in.end(); ++it) {//i=1 rear=1 prev=0->1 prev=1 ++i
			rear = new Node(*it);//i=2 rear=2 prev=1->2 prev=2 ++i
			prev->next = rear;
			prev = rear;
		}
	}
	//��������
	LinkQueue(const LinkQueue& other) 
		:front(nullptr), rear(nullptr) {
		if (other.front == nullptr) {
			return;
		}
		front = new Node(other.front->element);
		rear = front;
		Node* prev = front;
		for (Node* tempOther = other.front->next; tempOther != nullptr; tempOther = tempOther->next) {
			rear = new Node(tempOther->element);
			prev->next = rear;
			prev = rear;
		}
	}
	//�ƶ�����
	LinkQueue(LinkQueue&& other)noexcept 
		:front(other.front), rear(other.rear) {
		other.front = nullptr;
		other.rear = nullptr;
	}
	//�ƶ�����
	~LinkQueue() {
		clear();
	}
	//�������
	inline void clear() {
		_clearPtr(front);
	}
	//���ptr�Լ�֮�����������
	void _clearPtr(Node* ptr) {
		for (Node* next = nullptr; ptr != nullptr;) {//head=0->1->2->3 ptr=1
			next = ptr->next;//next=2 delete 1 ptr=next=2
			delete ptr;//next=3 delete 2  ptr=next=3
			ptr = next;//next=null delete 3 ptr=null end
		}
	}
	//������ֵ
	LinkQueue& operator=(const LinkQueue& other) {
		if (this != &other) {
			//�������
			clear();
			//��������
			if (other.front == nullptr) {
				return;
			}
			front = new Node(other.front->element);
			rear = front;
			Node* prev = front;
			for (Node* tempOther = other.front->next; tempOther != nullptr; tempOther = tempOther->next) {
				rear = new Node(tempOther->element);
				prev->next = rear;
				prev = rear;
			}
		}
		return *this;
	}
	//�ƶ���ֵ
	LinkQueue& operator=(LinkQueue&& other) noexcept {
		if (this != &other) {
			//�������
			clear();
			front = other.front;
			rear = other.rear;
			other.front = nullptr;
			other.rear = nullptr;
		}
		return *this;
	}
	//����������
	inline size_t size() const {
		size_t length = 0;//0->1->2
		for (Node* ptr = front; ptr != nullptr; ++length) { ptr = ptr->next; }//ptr=0 i=0;ptr=1 i=1;ptr=2 i=2;ptr=null i=3;end
		return length;
	}
	//�������Ϊ�� ����TRUE
	inline bool empty() const {
		return front == nullptr;
	}
	//���ض���Ԫ��
	inline T& back() {
		if (empty()) {
#ifdef DEBUG
			throw std::out_of_range("Queue is empty");
#endif // DEBUG
			return nullRef<T>();
		}
		return front->element;
	}
	//���ض���Ԫ��
	inline const T& back() const {
		if (empty()) {
#ifdef DEBUG
			throw std::out_of_range("Queue is empty");
#endif // DEBUG
			return nullRef<T>();
		}
		return front->element;
	}
	//���
	void push(const T& value) {
		if (front == nullptr) {
			front= new Node(value);
			rear = front;
			return;
		}
		Node* ptr = new Node(value);
		rear->next = ptr;
		rear = ptr;
		return;
	}
	//����
	void pop() {
		if (front == nullptr) {
			return;
		}
		Node* ptr = front->next;
		delete front;
		front = ptr;
		return;
	}
	//������Ԫ��ִ��func
	template<typename Function>
	inline void forEachFunction(Function func) {
		for (Node* ptr = front; ptr != nullptr; ptr = ptr->next) {
			func(ptr->element);
		}
	}
};


