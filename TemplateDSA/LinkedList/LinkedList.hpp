#pragma once
#include <initializer_list>
#include <functional>
template<typename T>
class LinkedList {
private:
	// ǰ��������������
	template<typename U>
	class LinkedListIterator;
	// �ڵ��ඨ��
	template<typename U>
	class _Node {
	public:
		using node_type = _Node<U>;
		friend class LinkedList;
		friend class LinkedListIterator<T>;
	private:
		U _data;
		node_type* _next;
	public:
		explicit _Node(const U& value, node_type* ptr = nullptr)
			: _data(value), _next(ptr) {}
		explicit _Node(U&& value, node_type* ptr = nullptr)
			: _data(std::move(value)), _next(ptr) {}
	};
public:
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
	using size_type = std::size_t;
	using node_type = _Node<T>;
	using iterator = LinkedListIterator<T>;
	using const_iterator = LinkedListIterator<const T>;
	friend class LinkedListIterator<T>;
	friend class LinkedListIterator<const T>;
private:
	node_type* _head;  // ͷָ��
	size_type _length; // ������
public:
	// ���� initialSize ��TypeĬ�Ϲ������
	/*
			size=length=3 head=0->1->2
			i=1 next=1 prev=0->1 prev=1 ++i
			i=2 next=2 prev=1->2 prev=2 ++i
			i=3 end
	*/
	LinkedList(size_type initialSize = 0)
		: _head(nullptr), _length(initialSize) {
		if (_length == 0) { return; }
		_head = new node_type(T());
		node_type* prev = _head;
		for (size_type i = 1; i < _length; ++i) {
			prev->_next = new node_type(T());
			prev = prev->_next;
		}
	}

	// ���� initialSize ��value����
	LinkedList(size_type initialSize, const T& value)
		: _head(nullptr), _length(initialSize) {
		if (_length == 0) { return; }
		_head = new node_type(value);
		node_type* prev = _head;
		for (size_type i = 1; i < _length; ++i) {
			prev->_next = new node_type(value);
			prev = prev->_next;
		}
	}

	// ��������Χ���캯��
	template<typename InputIt>
	LinkedList(InputIt first, InputIt last)
		: _head(nullptr), _length(std::distance(first, last)) {
		if (_length == 0) { return; }
		_head = new node_type(*first);
		node_type* prev = _head;
		for (++first; first != last; ++first) {
			prev->_next = new node_type(*first);
			prev = prev->_next;
		}
	}

	// ��ʼ���б���
	LinkedList(std::initializer_list<T> init)
		: LinkedList(init.begin(), init.end()) {}; // ί�и����乹�캯��

	// �������캯��
	LinkedList(const LinkedList& other)
		: _head(nullptr), _length(other._length) {
		if (other._head == nullptr) {
			_length = 0;
			return;
		}
		_head = new node_type(other._head->_data);
		node_type* prev = _head;
		for (node_type* otherPrev = other._head->_next;
			otherPrev != nullptr;
			otherPrev = otherPrev->_next) {
			prev->_next = new node_type(otherPrev->_data);
			prev = prev->_next;
		}
	}

	// �ƶ����캯��
	LinkedList(LinkedList&& other)noexcept
		: _head(other._head), _length(other._length) {
		other._head = nullptr;
		other._length = 0;
	}

	// ������ֵ����
	LinkedList& operator=(const LinkedList& other) {
		if (this != &other) {
			clear();
			if (other._head == nullptr) {
				_length = 0;
				return *this;
			}
			_head = new node_type(other._head->_data);
			node_type* prev = _head;
			for (node_type* otherPrev = other._head->_next;
				otherPrev != nullptr;
				otherPrev = otherPrev->_next) {
				prev->_next = new node_type(otherPrev->_data);
				prev = prev->_next;
			}
			_length = other._length;
		}
		return *this;
	}

	// �ƶ���ֵ����
	LinkedList& operator=(LinkedList&& other) noexcept {
		if (this != &other) {
			clear();
			_head = other._head;
			_length = other._length;
			other._head = nullptr;
			other._length = 0;
		}
		return *this;
	}

	// ֧�־ۺϳ�ʼ����ֵ
	LinkedList& operator=(std::initializer_list<T> init) {
		LinkedList temp(init);
		swap(*this, temp);
		return *this;
	}

	~LinkedList() {
		clear();
	}
public:

	// ������λԪ��
	T& front() {
		if (empty()) throw std::out_of_range("LinkedList is empty");
		return _head->_data;
	}
	const T& front() const noexcept {
		if (empty()) throw std::out_of_range("LinkedList is empty");
		return _head->_data;
	}

	// �����λԪ��
	void push_front(const T& value) {
		_head = new node_type(value, _head);
		++_length;
		return;
	}
	void push_front(T&& value) {
		_head = new node_type(std::move(value), _head);
		++_length;
		return;
	}
	// ������λԪ��
	template<typename... Args>
	void emplace_front(Args&&... args) {
		_head = new node_type(T(std::forward<Args>(args)...), _head);
		++_length;
		if (_length == 1) _tail = _head;
	}
	
	// �Ƴ���λԪ��
	void pop_front() {
		if (empty()) throw std::out_of_range("LinkedList is empty");
		node_type* toDelete = _head;
		_head = _head->_next;
		delete toDelete;
		--_length;
	}

	// ��ĳ��Ԫ�غ������Ԫ�� ���ص�����ָ����Ԫ��
	iterator insert_after(iterator pos, const T& value) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		node_type* new_node = new node_type(value, pos.ptr->next);
		pos.ptr->next = new_node;
		++_length;
		return iterator(new_node);
	}
	iterator insert_after(iterator pos, T&& value) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		node_type* new_node = new node_type(std::move(value), pos.ptr->next);
		pos.ptr->next = new_node;
		++_length;
		return iterator(new_node);
	}
	// ����������
	template<typename... Args>
	iterator emplace_after(iterator pos, Args&&... args) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		node_type* new_node = new node_type(T(std::forward<Args>(args)...), pos.ptr->next);
		pos.ptr->next = new_node;
		++_length;
		return iterator(new_node);
	}
	// �����������Ԫ��
	void insert(size_type index, const T& value) {
		if (index > _length) throw std::out_of_range("Invalid position");
		if (index == 0) {
			push_front(value);
			return;
		}
		node_type* prev = _getNodeAt(index - 1);
		insert_after(iterator(prev), value);
	}
	void insert(size_type index, T&& value) {
		if (index > _length) throw std::out_of_range("Invalid position");
		if (index == 0) {
			push_front(std::move(value));
			return;
		}
		node_type* prev = _getNodeAt(index - 1);
		insert_after(iterator(prev), std::move(value));
	}
	// ���������첢����Ԫ��
	template<typename... Args>
	void emplace(size_type index, Args&&... args) {
		if (index > _length) throw std::out_of_range("Invalid position");
		if (index == 0) {
			emplace_front((T(std::forward<Args>(args)...));
			return;
		}
		node_type* prev = _getNodeAt(index - 1);
		emplace_after(iterator(prev), (T(std::forward<Args>(args)...));
	}

	// ��ĳ��Ԫ�غ�ɾ��Ԫ�� ���ص�����ָ��ɾ���ĺ�һ��Ԫ��
	iterator erase_after(iterator pos) {
		if (pos == end() || pos.ptr->next == nullptr)
			throw std::out_of_range("Invalid position");
		node_type* toDelete = pos.ptr->next;
		pos.ptr->next = toDelete->next;
		delete toDelete;
		--_length;
		return iterator(pos.ptr->next);
	}
	// ɾ������Ԫ��
	void erase(size_type index) {
		if (index >= _length) throw std::out_of_range("Invalid position");
		if (index == 0) {
			node_type* toDelete = _head;
			_head = _head->next;
			delete toDelete;
			--_length;
			return begin();
		}
		node_type* prev = _getNodeAt(index - 1);
		return erase_after(iterator(prev));
	}


	// ��ȫ����
	T& at(size_type index) {
		if (index >= _length) throw std::out_of_range("Invalid position");
		return _getNodeAt(index)->_data;
	}
	const T& at(size_type index) const {
		if (index >= _length) throw std::out_of_range("Invalid position");
		return _getNodeAt(index)->_data;
	}

	// �ض��������ؽض�����
	LinkedList cut_after(iterator pos) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		LinkedList newList;
		if (pos.ptr->next == nullptr) return newList;
		newList._head = pos.ptr->next;
		pos.ptr->next = nullptr;
		// ��ȡ����
		size_type cutLength = 0;
		node_type* node = newList._head;
		while (node != nullptr) {
			++cutLength;
			node = node->next;
		}
		newList._length = cutLength;
		_length -= cutLength;
		return newList;
	}
	LinkedList cut(size_type index) {
		if (index >= _length) throw std::out_of_range("Invalid position");
		if (index == 0) {
			LinkedList newList;
			newList._head = _head;
			newList._length = _length;
			_head = nullptr;
			_length = 0;
			return newList;
		}
		node_type* prev = _getNodeAt(index - 1);
		return cut_after(iterator(prev));
	}

	// �ж�Ϊ���� �ƶ��ν���������������������
	LinkedList link_after(iterator pos, LinkedList&& other) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		if (other.empty()) return;
		node_type* oldNext = pos.ptr->next;
		pos.ptr->next = other._head;
		node_type* tail = other._head;
		while (tail->next != nullptr) {
			tail = tail->next;
		}
		tail->next = oldNext;
		_length += other._length;
		other._head = nullptr;
		other._length = 0;
	}
	LinkedList link(size_type index, LinkedList&& other) {
		if (index > _length) throw std::out_of_range("Invalid position");
		if (index == 0) {
			if (empty()) {
				*this = std::move(other);
			}
			else {
				node_type* tail = other._head;
				while (tail != nullptr && tail->next != nullptr) {
					tail = tail->next;
				}
				if (tail != nullptr) {
					tail->next = _head;
					_head = other._head;
					_length += other._length;
					other._head = nullptr;
					other._length = 0;
				}
			}
			return;
		}
		node_type* prev = _getNodeAt(index - 1);
		link_after(iterator(prev), std::move(other));
	}

	// ��ѯ�Ƿ�Ϊ��
	bool empty() const noexcept { return _length == 0; }

	// ��ȡ������
	size_type size() const noexcept { return _length; }

	//����ȫ����
	void clear() {
		node_type* temp;
		while (_head) {
			temp = _head;
			_head = _head->_next;
			delete temp;
		}
		_length = 0;
	}

	// Ԥ�����ڴ棨��ʵ�����ã���Ϊ����STL�ӿڣ�
	void reserve(size_type new_cap) { /* ��������Ԥ���ռ� */ }

	// ������С
	void resize(size_type count, const value_type& value = value_type()) {
		if (_length > count) {// ������С - ʹ�� _clearPtr һ���Խض�
			if (count == 0) {
				clear();
			}
			else {
				node_type* prev = _getNodeAt(count - 1);
				_clearPtr(prev); // ��� prev ֮������нڵ�
				prev->_next = nullptr;
				_tail = prev;   // ����βָ��
				_length = count;
			}
			return;
		}
		if (_length < count) {// �����С - ��������
			size_type to_add = count - _length;
			LinkedList temp(to_add, value); // ������ʱ����
			if (empty()) {
				*this = std::move(temp);   // ֱ�ӽӹ���ʱ����
			}
			else {
				_tail->_next = temp._head;  // ���ӵ�β��
				_tail = temp._getNodeAt(to_add - 1); // ����βָ��
				_length = count;
				temp._head = nullptr;       // ��ֹ��ʱ��������ʱɾ���ڵ�
				temp._length = 0;
			}
		}
	}

private:
	// ���ptr������������
	void _clearPtr(node_type* ptr) {
		for (node_type* tempPtr = ptr->_next //head=0->1->2->3 ptr=0
			; ptr != nullptr;) {    //ptr=1 next=2 delete 1
			ptr = tempPtr;          //ptr=2 next=3 delete 2
			tempPtr = ptr->_next; //ptr=3 next=null delete 3
			delete ptr;             //ptr=null
			--_length;
		}
	}

	// ��ȡ������Ӧָ��
	node_type* _getNodeAt(size_type index) const {
		if(index >= _length) throw std::out_of_range("Invalid position");
		node_type* node = _head;
		for (size_type i = 0; i < index; ++i) {
			node = node->next;
		}
		return node;
	}

public:
	template<typename T>
	void swap(LinkedList<T>& lhs, LinkedList<T>& rhs) noexcept {
		lhs.swap(rhs);
	}

	friend void swap(LinkedList& other) noexcept {
		using std::swap;
		swap(_head, other._head);
		swap(_length, other._length);
	}
public:
	// ��������ز���
	iterator begin() noexcept { return iterator(_head); }
	iterator end() noexcept { return iterator(nullptr); }
	iterator before_end() noexcept {
		return empty() ? end() : iterator(_getNodeAt(_length - 1));
	}
	const_iterator cbegin() const noexcept { return begin(); }
	const_iterator cend() const noexcept { return end(); }
	const_iterator cbefore_end() const noexcept {return before_end();}
};
//LinkedList��
template<typename T>
template<typename U>
class LinkedList<T>::LinkedListIterator {
public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = U;
	using difference_type = std::ptrdiff_t;
	using pointer = U*;
	using reference = U&;
	using node_type = LinkedList<T>::_Node<U>;
	using iterator = LinkedList<T>::LinkedListIterator;
	friend class LinkedList<T>;
private:
	node_type* _ptr;// ָ������ڵ��ָ��
public:
	explicit LinkedListIterator(node_type* ptr) : _ptr(ptr) {}

	T& operator*() const { return _ptr->_data; }
	T* operator->() const { return &_ptr->_data; }

	iterator& operator++() {
		_ptr = _ptr->_next;
		return *this;
	}

	iterator operator++(int) {
		iterator tmp = *this;
		++(*this);
		return tmp;
	}

	bool operator==(const iterator& other) const {
		return _ptr == other._ptr;
	}
	bool operator!=(const iterator& other) const {
		return !(*this == other);
	}
};