#pragma 
#include "myHead.h"

template<typename T>
class DoubleLinkedList {
protected:
	template<typename T>
	struct ListNode {//����ṹ
		using Node = ListNode<T>;
		T element;
		Node* prev;
		Node* next;
		ListNode(const T& in, Node* inPrev = nullptr, Node* inNext = nullptr) :element(in), prev(inPrev), next(inNext) {};
		ListNode(T&& in, Node* inPrev = nullptr, Node* inNext = nullptr) :element(std::move(in)), prev(inPrev), next(inNext) {};
	};
	using Node = ListNode<T>;
	Node* head;  //����ͷָ��  
	Node* tail;  //����βָ��  
	size_t length;//����
public:
	//���캯��
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
	//����size��inԪ��
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
	//�б��ʼ��
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
	//��������
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
	//�ƶ�����
	DoubleLinkedList(DoubleLinkedList&& other)noexcept :head(other.head), tail(other.tail), length(other.length) {
		other.head = nullptr;
		other.tail = nullptr;
		other.length = NULL;
	}
	//�ƶ�����
	~DoubleLinkedList() {
		clear();
	}
	//�������
	void clear() {
		_clearPtr(head,0);
	}
	//���ptr�Լ�֮���֮ǰ����������
	void _clearPtr(Node* ptr,int model) {
		if (model == 0) {//��������
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
	//������ֵ
	DoubleLinkedList& operator=(const DoubleLinkedList& other) {
		//�������
		if (this != &other) {
			clear();
			//��������
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
	//�ƶ���ֵ
	DoubleLinkedList& operator=(DoubleLinkedList&& other) noexcept {
		//�������
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
	//У׼������
	size_t _alignLength(int model) {//head=0<->1<->2
		length = 0;
		if (model == 0) {//�����������
			for (Node* ptr = head; ptr != nullptr; ++length) { ptr = ptr->next; }//ptr=0 i=0;ptr=1 i=1;ptr=2 i=2;ptr=null i=3;end
		}
		else {
			for (Node* ptr = tail; ptr != nullptr; ++length) { ptr = ptr->prev; }//ptr=2 i=0;ptr=1 i=1;ptr=0 i=2;ptr=null i=3;end
		}
		return length;
	}
	//����������
	size_t size() const {
		return length;
	}
	//�������Ϊ�� ����TRUE
	bool empty() const {
		return head == nullptr;
	}
	//��ת����
	void reverse() {
		if (head == nullptr || head->next == nullptr) {
			return; // �������ֻ��һ���ڵ㣬���跴ת
		}
		tail = head;//����β�ڵ�ָ��Ϊԭ�������ͷ�ڵ�
		Node* tempptr = nullptr;
		while (head->next != nullptr) {
			tempptr = head->next;
			head->next = head->prev;
			head->prev = tempptr;
			head = tempptr;
		}
		head->next = head->prev;
	}
	//����ָ��ָ��Ԫ��
	static T& getElem(Node* ptr) {
		return ptr->element;
	}
	static const T& getElem(Node* ptr) const {
		return ptr->element;
	}
	//����ָ����
	static Node* getNext(Node* ptr) {
		return ptr->next;
	}
	//����ָ��ǰ��
	static Node* getPrev(Node* ptr) {
		return ptr->prev;
	}
	//���ʵ�index����ָ��
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
	//���ʵ�index����ָ��
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
	//����Ԫ������
	T& operator[](size_t index) {
		return  _atPtr(index)->element;
	}
	//����Ԫ������
	const T& operator[](size_t index) const {
		return  _atPtr(index)->element;
	}
	//��ָ������ǰ������Ԫ��
	//ָ��ָ����ܲ��Ǳ�����ָ��,С��ʹ��
	//�����ڲ�����,��û�ж�length�޸�
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
	//��ָ�������̲���Ԫ��
	//ָ��ָ����ܲ��Ǳ�����ָ��,С��ʹ��
	//�����ڲ�����,��û�ж�length�޸�
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
	//��ָ��ɾ������
	//ָ��ָ����ܲ��Ǳ�����ָ��,С��ʹ��
	void _removePtr(const Node*& ptr) {
		if (ptr != nullptr) {//2<->3<->4 ptr=3
			if(ptr->prev != nullptr)
				ptr->prev->next = ptr->next;//2->4
			if(ptr->next != nullptr)
				ptr->next->prev = ptr->prev;//2<-4
			delete ptr;
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
	//ɾ��ָ������λ�õ�Ԫ��  
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
	//ĩβ����Ԫ��
	void push_back(const T& value) {
		tail->next = new Node(value,tail,nullptr);//tail=2 2<->new->Null
		tail = tail->next;//tail=new
		++length;
	}
	//ĩβ�Ƴ�Ԫ��
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
	//��λ���Ԫ��
	void push_front(const T& value) {
		head->prev = new Node(value,nullptr, head);//head=0 Null<-new<->0
		head = head->prev;//head=new
		++length;
	}
	//��λ�Ƴ�Ԫ��
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
	//������Ԫ��ִ��func
	template<typename Function>
	void forEachFunction(Function func) {
		for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) {
			func(ptr->element);
		}
	}
	//������Ԫ������ִ��func
	template<typename Function>
	void forEachFunctionInReverse(Function func) {
		for (Node* ptr = tail; ptr != nullptr; ptr = ptr->prev) {
			func(ptr->element);
		}
	}
	//Ѱ�ҵ�һ������cond��Ԫ�ص�����
	template<typename Condition>
	Node* findFirstByCondition(Condition cond) {
		for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) {
			if (cond(ptr->element))
				return ptr;
		}
		return nullptr;
	}
	//Ѱ�����һ������cond��Ԫ�ص�����
	template<typename Condition>
	Node* findLostByCondition(Condition cond) {
		for (Node* ptr = tail; ptr != nullptr; ptr = ptr->prev) {
			if (cond(ptr->element))
				return ptr;
		}
		return nullptr;
	}
	//�Է���condԪ��ִ��func
	template<typename Condition, typename Function>
	void forEachFunctionByCondition(Condition cond, Function func) {
		for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) {
			if (cond(ptr->element))
				func(ptr->element);
		}
	}
	//����Է���condԪ��ִ��func
	template<typename Condition, typename Function>
	void forEachFunctionByConditionInReverse(Condition cond, Function func) {
		for (Node* ptr = tail; ptr != nullptr; ptr = ptr->prev) {
			if (cond(ptr->element))
				func(ptr->element);
		}
	}
	//��index��ʼ����Ѱ����value���Ԫ��,��������ָ��,���û�з���nullptr
	Node* findPtrInOrder(const T& value, size_t index = 0) {//head=0-5-3-2-1-4 index=2 value=4
		for (Node* ptr = _atPtr(index); ptr != nullptr; ptr = ptr->next) {//i=2 ptr=3;i=3 ptr=2;i=4 ptr=1;i=5 ptr=4;end
			if (ptr->element == value) {
				break;
			}
		}
		return ptr;
	}
	//��index��ʼ����Ѱ����value���Ԫ��,������������,���û�з���length
	size_t findIndexInOrder(const T& value, size_t startIndex = 0) {//head=0-5-3-2-1-4 index=2 value=4
		for (Node* ptr = _atPtr(startIndex); ptr != nullptr; ptr = ptr->next, ++startIndex) {//i=2 ptr=3;i=3 ptr=2;i=4 ptr=1;i=5 ptr=4;end
			if (ptr->element == value) {
				break;
			}
		}
		return startIndex;
	}
	//��index��ʼ����Ѱ����value���Ԫ��,������������,���û�з���length
	Node* findPtrInReverse(const T& value, size_t index = 0) {//head=0-5-3-2-1-4 index=3 value=3
		for (Node* ptr = _atPtr(index); ptr != nullptr; ptr = ptr->prev, --index) {//i=3 ptr=2;i=2 ptr=3;end
			if (ptr->element == value) {
				break;
			}
		}
		return ptr;
	}
	//��index��ʼ����Ѱ����value���Ԫ��,������������,���û�з���length
	size_t findIndexInReverse(const T& value, size_t startIndex = 0) {//head=0-5-3-2-1-4 index=3 value=3
		for (Node* ptr = _atPtr(startIndex); ptr != nullptr; ptr = ptr->prev, --startIndex) {//i=3 ptr=2;i=2 ptr=3;end
			if (ptr->element == value) {
				break;
			}
		}//i=2 ptr=2;i=3 ptr=3;i=4 ptr=4;end
		return startIndex;
	}
};

