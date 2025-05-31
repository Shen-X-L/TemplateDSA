#pragma once
#include <initializer_list>
#include <functional>
template<typename T>
class LinkedList {
private:
	// 前向声明迭代器类
	template<typename U>
	class LinkedListIterator;
	// 节点类定义
	template<typename U>
	class _Node {
	public:
		using node_type = _Node<U>;
		friend class LinkedList;
		friend class LinkedListIterator<T>;
		friend class LinkedListIterator<const T>;
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
	node_type* _head = nullptr;  // 头指针
	size_type _length = 0; // 链表长度
public:
	LinkedList() noexcept = default;
	// 构造 initialSize 个Type默认构造对象
	/*
			size=length=3 head=0->1->2
			i=1 next=1 prev=0->1 prev=1 ++i
			i=2 next=2 prev=1->2 prev=2 ++i
			i=3 end
	*/
	LinkedList(size_type initialSize)
		: _head(nullptr), _length(initialSize) {
		if (_length == 0) { return; }
		_head = new node_type(T());
		node_type* prev = _head;
		for (size_type i = 1; i < _length; ++i) {
			prev->_next = new node_type(T());
			prev = prev->_next;
		}
	}

	// 构造 initialSize 个value对象
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

	// 迭代器范围构造函数
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

	// 初始化列表构造
	LinkedList(std::initializer_list<T> init)
		: LinkedList(init.begin(), init.end()) {}; // 委托给区间构造函数

	// 拷贝构造函数
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

	// 移动构造函数
	LinkedList(LinkedList&& other)noexcept
		: _head(other._head), _length(other._length) {
		other._head = nullptr;
		other._length = 0;
	}

	// 拷贝赋值函数
	LinkedList& operator=(const LinkedList& other) {
		if (this != &other) {
			clear();
			if (other._head == nullptr) {
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

	// 移动赋值函数
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

	// 支持聚合初始化赋值
	LinkedList& operator=(std::initializer_list<T> init) {
		LinkedList temp(init);
		swap(*this, temp);
		return *this;
	}

	~LinkedList() {
		clear();
	}
public:

	// 返回首位元素
	T& front() {
		if (empty()) throw std::out_of_range("LinkedList is empty");
		return _head->_data;
	}
	const T& front() const noexcept {
		if (empty()) throw std::out_of_range("LinkedList is empty");
		return _head->_data;
	}

	// 添加首位元素
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
	// 构造首位元素
	template<typename... Args>
	void emplace_front(Args&&... args) {
		_head = new node_type(T(std::forward<Args>(args)...), _head);
		++_length;
	}
	
	// 移除首位元素
	void pop_front() {
		if (empty()) throw std::out_of_range("LinkedList is empty");
		node_type* toDelete = _head;
		_head = _head->_next;
		delete toDelete;
		--_length;
	}

	// 在某个元素后插入新元素 返回迭代器指向新元素
	iterator insert_after(iterator pos, const T& value) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		node_type* new_node = new node_type(value, pos.ptr->_next);
		pos.ptr->_next = new_node;
		++_length;
		return iterator(new_node);
	}
	iterator insert_after(iterator pos, T&& value) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		node_type* new_node = new node_type(std::move(value), pos.ptr->_next);
		pos.ptr->_next = new_node;
		++_length;
		return iterator(new_node);
	}
	// 构造插入对象
	template<typename... Args>
	iterator emplace_after(iterator pos, Args&&... args) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		node_type* new_node = new node_type(T(std::forward<Args>(args)...), pos.ptr->_next);
		pos.ptr->_next = new_node;
		++_length;
		return iterator(new_node);
	}
	// 在索引位置 插入元素
	void insert(size_type index, const T& value) {
		if (index > _length) throw std::out_of_range("Invalid position");
		if (index == 0) {
			push_front(std::move(value));
			return;
		}
		node_type* prev = _get_node_at(index - 1);
		insert_after(iterator(prev), value);
	}
	void insert(size_type index, T&& value) {
		if (index > _length) throw std::out_of_range("Invalid position");
		if (index == 0) {
			push_front(std::move(value));
			return;
		}
		node_type* prev = _get_node_at(index - 1);
		insert_after(iterator(prev), std::move(value));
	}
	// 在索引后构造并插入元素
	template<typename... Args>
	void emplace(size_type index, Args&&... args) {
		if (index > _length) throw std::out_of_range("Invalid position");
		if (index == 0) {
			emplace_front(std::forward<Args>(args));
			return;
		}
		node_type* prev = _get_node_at(index - 1);
		emplace_after(iterator(prev), std::forward<Args>(args));
	}

	// 在某个元素后删除元素 返回迭代器指向被删除的后一个元素
	iterator erase_after(iterator pos) {
		if (pos == end() || pos.ptr->_next == nullptr)
			throw std::out_of_range("Invalid position");
		node_type* toDelete = pos.ptr->_next;
		pos.ptr->_next = toDelete->_next;
		delete toDelete;
		--_length;
		return iterator(pos.ptr->_next);
	}
	// 删除索引元素
	void erase(size_type index) {
		if (index >= _length) throw std::out_of_range("Invalid position");
		if (index == 0) {
			node_type* toDelete = _head;
			_head = _head->_next;
			delete toDelete;
			--_length;
			return ;
		}
		node_type* prev = _get_node_at(index - 1);
		erase_after(iterator(prev));
		return;
	}


	// 安全访问
	T& at(size_type index) {
		if (index >= _length) throw std::out_of_range("Invalid position");
		return _get_node_at(index)->_data;
	}
	const T& at(size_type index) const {
		if (index >= _length) throw std::out_of_range("Invalid position");
		return _get_node_at(index)->_data;
	}

	// 在迭代器与迭代器下一个元素之间 截断链表并返回截断链表
	LinkedList cut_after(iterator pos) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		LinkedList newList;
		if (pos.ptr->_next == nullptr) return newList;
		newList._head = pos.ptr->_next;
		pos.ptr->_next = nullptr;
		// 获取长度
		size_type cutLength = 0;
		node_type* node = newList._head;
		while (node != nullptr) {
			++cutLength;
			node = node->_next;
		}
		newList._length = cutLength;
		_length -= cutLength;
		return newList;
	}
	// 在索引和索引上一个元素之间 截断链表并返回截断链表
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
		node_type* prev = _get_node_at(index - 1);
		LinkedList newList;
		newList._head = prev->_next;
		newList._length = _length - index;
		prev->_next = nullptr;
		_length = index;
		return newList;
	}

	// 在迭代器与迭代器下一个元素之间 中断为两节 移动衔接其他链表再重链接两节
	LinkedList& splice_after(iterator pos, LinkedList&& other) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		if (other.empty()) return;
		node_type* oldNext = pos.ptr->_next;
		pos.ptr->_next = other._head;
		node_type* tail = other._head;
		while (tail->_next != nullptr) {
			tail = tail->_next;
		}
		tail->_next = oldNext;
		_length += other._length;
		other._head = nullptr;
		other._length = 0;
		return *this;
	}
	template<typename InputIt>
	LinkedList& splice_after(iterator pos, InputIt first, InputIt end) {
		LinkedList temp = LinkedList(first, end);
		return splice_after(pos, std::move(temp));
	}
	// 在索引和索引上一个元素之间 中断为两节 移动衔接其他链表再重链接两节
	LinkedList& splice(size_type index, LinkedList&& other) {
		if (index > _length) throw std::out_of_range("Invalid position");
		if (index == 0) {
			return splice_front(other);
		}
		node_type* prev = _get_node_at(index - 1);
		return splice_after(iterator(prev), std::move(other));
	}
	LinkedList& splice_front(LinkedList&& other) {
		if (empty()) {
			*this = std::move(other);
		} else {
			node_type* tail = other._head;
			if (tail != nullptr) {
				while (tail->_next != nullptr) {
					tail = tail->_next;
				}
				tail->_next = _head;
				_head = other._head;
				_length += other._length;
				other._head = nullptr;
				other._length = 0;
			}
		}
		return *this;
	}

	// 查询是否为空
	bool empty() const noexcept { return _length == 0; }

	// 获取链表长度
	size_type size() const noexcept { return _length; }

	//清理全链表
	void clear() {
		node_type* temp;
		while (_head) {
			temp = _head;
			_head = _head->_next;
			delete temp;
		}
		_length = 0;
	}

	// 预分配内存（无实际作用，仅为兼容STL接口）
	void reserve(size_type new_cap) { /* 链表无需预留空间 */ }

	// 调整大小
	void resize(size_type count, const value_type& value = value_type()) {
		if (_length > count) {// 缩减大小 - 使用 _clearPtr 一次性截断
			if (count == 0) {
				clear();
			}
			else {
				node_type* prev = _get_node_at(count - 1);
				_clear_after(prev); // 清除 prev 之后的所有节点
				_length = count;
			}
			return;
		}
		if (_length < count) {// 增大大小 - 批量插入
			size_type to_add = count - _length;
			LinkedList temp(to_add, value); // 创建临时链表
			if (empty()) {
				*this = std::move(temp);   // 直接接管临时链表
			}
			else {
				_get_node_at(_length - 1)->_next = temp._head;  // 链接到尾部
				_length = count;
				temp._head = nullptr;       // 防止临时链表析构时删除节点
				temp._length = 0;
			}
		}
	}

private:
	// 清除ptr后续所有链表
	/*
	head=0->1->2->3 ptr=0
	ptr=1 next=2 delete 1
	ptr=2 next=3 delete 2
	ptr=3 next=null delete 3
	*/
	void _clear_after(node_type* ptr) {
		node_type* temp = ptr;
		for (node_type* tempPtr = ptr->_next; ptr != nullptr;) {   
			ptr = tempPtr;         
			tempPtr = ptr->_next;
			delete ptr;
			--_length;
		}
		temp->_next = nullptr;
	}

	// 获取索引对应指针
	node_type* _get_node_at(size_type index) const {
		if(index >= _length) throw std::out_of_range("Invalid position");
		node_type* node = _head;
		for (size_type i = 0; i < index; ++i) {
			node = node->_next;
		}
		return node;
	}

public:
	void swap(LinkedList& lhs, LinkedList& rhs) noexcept {
		lhs.swap(rhs);
	}

	friend void swap(LinkedList& other) noexcept {
		using std::swap;
		swap(_head, other._head);
		swap(_length, other._length);
	}
public:
	// 迭代器相关操作
	iterator begin() noexcept { return iterator(_head); }
	iterator end() noexcept { return iterator(nullptr); }
	iterator before_end() noexcept {
		return empty() ? end() : iterator(_get_node_at(_length - 1));
	}
	const_iterator cbegin() const noexcept { return begin(); }
	const_iterator cend() const noexcept { return end(); }
	const_iterator cbefore_end() const noexcept {return before_end();}
};
//LinkedList内
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
	using iterator = LinkedListIterator<T>;
	using const_iterator = LinkedListIterator<const T>;
	friend class LinkedList<T>;
private:
	node_type* _ptr;// 指向链表节点的指针
public:
	explicit LinkedListIterator(node_type* ptr) : _ptr(ptr) {}

	reference operator*() const { return _ptr->_data; }
	pointer operator->() const { return &_ptr->_data; }

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

private:
	node_type* node() const { return _ptr; }
};