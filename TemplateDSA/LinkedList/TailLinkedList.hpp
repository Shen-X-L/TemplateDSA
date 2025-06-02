#pragma once
template<typename T>
class TailLinkedList {
private:
	// 前向声明迭代器类
	template<typename U>
	class TailLinkedListIterator;

	// 节点类定义
	template<typename U>
	class _Node {
	public:
		using node_type = _Node<U>;
		friend class TailLinkedList;
		friend class TailLinkedListIterator<T>;
		friend class TailLinkedListIterator<const T>;
	protected:
		node_type* _next;
		U _data;
	public:
		explicit _Node(const U& value, node_type* ptr = nullptr)
			: _data(value), _next(ptr) {}
		explicit _Node(U&& value, node_type* ptr = nullptr)
			: _data(std::move(value)), _next(ptr) {}
	};
	// 哨兵节点（用于before_begin）
	struct SentinelNode:public _Node<T> {
		SentinelNode() : _Node<T>(T(), nullptr) {}
		SentinelNode(const SentinelNode& other) : _Node<T>(T(), nullptr) {}
		SentinelNode(SentinelNode&& other) : _Node<T>(T(), other._next) {
			other._next = nullptr; // 确保移动后源对象为空
		}
	};
public:
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
	using size_type = std::size_t;
	using node_type = _Node<T>;
	using iterator = TailLinkedListIterator<T>;
	using const_iterator = TailLinkedListIterator<const T>;
	friend class TailLinkedListIterator<T>;
	friend class TailLinkedListIterator<const T>;
private:
	SentinelNode _sentinel = SentinelNode();  // 哨兵节点
	node_type* _tail = nullptr;       // 尾指针
	size_type _length = 0; // 链表长度
public:
	TailLinkedList() noexcept = default;
	// 填充构造函数（n个默认值）
	explicit TailLinkedList(size_type n) {
		for (size_type i = 0; i < n; ++i) {
			push_back(T()); // 默认构造元素
		}
	}

	// 构造 n 个value对象
	TailLinkedList(size_type n, const T& value) {
		for (size_type i = 0; i < n; ++i) {
			push_back(value); // 默认构造元素
		}
	}

	// 迭代器范围构造函数
	template<typename InputIt>
	TailLinkedList(InputIt first, InputIt last){
		for (; first != last; ++first) {
			push_back(*first);
		}
	}

	// 初始化列表构造
	TailLinkedList(std::initializer_list<T> init)
		: TailLinkedList(init.begin(), init.end()) {}; // 委托给区间构造函数

	// 拷贝构造函数
	TailLinkedList(const TailLinkedList& other):TailLinkedList(other.begin(), other.end()) {}

	//移动构造函数
	TailLinkedList(TailLinkedList&& other) noexcept
		: _sentinel(std::move(other._sentinel)),
		_tail(std::move(other._tail)),
		_length(std::move(other._length)) {
		other._sentinel._next = nullptr;
		other._tail = nullptr;
		other._length = 0;
	}

	// 拷贝赋值函数
	TailLinkedList& operator=(const TailLinkedList& other) {
		if (this != &other) {
			TailLinkedList temp(other);
			swap(temp);
		}
		return *this;
	}

	// 移动赋值函数
	TailLinkedList& operator=(TailLinkedList&& other) noexcept {
		if (this != &other) {
			clear();
			_sentinel._next = other._sentinel._next;
			_tail = other._tail;
			_length = other._length;
			other._sentinel._next = nullptr;
			other._tail = nullptr;
			other._length = 0;
		}
		return *this;
	}

	// 支持聚合初始化赋值
	TailLinkedList& operator=(std::initializer_list<T> init) {
		TailLinkedList temp(init);
		swap(temp);
		return *this;
	}

	~TailLinkedList() {
		clear();
	}
public:
	// 首元素访问
	T& front() {
		if (empty()) throw std::out_of_range("LinkedList is empty");
		return _sentinel._next->data; // 哨兵的next即首元素
	}
	const T& front() const noexcept {
		return const_cast<TailLinkedList*>(this)->front(); // 复用非const版本
	}

	// 尾元素访问
	T& back() {
		if (empty()) throw std::out_of_range("LinkedList is empty");
		return _tail->data; // 直接使用尾指针
	}
	const T& back() const noexcept {
		return const_cast<TailLinkedList*>(this)->back();
	}

	// 头部插入
	void push_front(const T& value) {
		_sentinel._next = new node_type(value, _sentinel._next);
		if (!_tail) _tail = _sentinel._next; // 原链表为空时更新尾指针
		++_length;
	}
	void push_front(T&& value) {
		_sentinel._next = new node_type(std::move(value), _sentinel._next);
		if (!_tail) _tail = _sentinel._next;
		++_length;
	}

	// 尾部插入
	void push_back(const T& value) {
		node_type* new_node = new node_type(value);
		if (empty()) {
			_sentinel._next = _tail = new_node;
		}
		else {
			_tail->_next = new_node;
			_tail = new_node;
		}
		++_length;
	}
	void push_back(T&& value) {
		node_type* new_node = new node_type(std::move(value));
		if (empty()) {
			_sentinel._next = _tail = new_node;
		}
		else {
			_tail->_next = new_node;
			_tail = new_node;
		}
		++_length;
	}

	// 头部删除
	void pop_front() {
		if (empty()) throw std::out_of_range("LinkedList is empty");
		node_type* to_delete = _sentinel._next;
		_sentinel._next = to_delete->_next;
		if (_tail == to_delete) _tail = nullptr; // 删除最后一个节点
		delete to_delete;
		--_length;
	}

	// 在某个元素后插入新元素 返回迭代器指向新元素
	iterator insert_after(iterator pos, const T& value) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		node_type* new_node = new node_type(value, pos._ptr->_next);
		pos._ptr->_next = new_node;
		++_length;
		return iterator(new_node);
	}
	iterator insert_after(iterator pos, T&& value) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		node_type* new_node = new node_type(std::move(value), pos._ptr->_next);
		pos._ptr->_next = new_node;
		++_length;
		return iterator(new_node);
	}
	// 在索引位置 插入元素
	void insert(size_type index, const T& value) {
		if (index > _length) throw std::out_of_range("Invalid position");
		insert_after(_get_pre_iterator(index), value);
	}
	void insert(size_type index, T&& value) {
		if (index > _length) throw std::out_of_range("Invalid position");
		insert_after(_get_pre_iterator(index), std::move(value));
	}

	// 构造插入对象
	template<typename... Args>
	iterator emplace_after(iterator pos, Args&&... args) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		node_type* new_node = new node_type(T(std::forward<Args>(args)...), pos._ptr->_next);
		pos._ptr->_next = new_node;
		++_length;
		return iterator(new_node);
	}
	// 在索引后构造并插入元素
	template<typename... Args>
	void emplace(size_type index, Args&&... args) {
		if (index > _length) throw std::out_of_range("Invalid position");
		emplace_after(_get_pre_iterator(index), std::forward<Args>(args)...);
	}

	// 在某个元素后删除元素 返回迭代器指向被删除的后一个元素
	iterator erase_after(iterator pos) {
		if (pos == end() || pos._ptr->_next == nullptr)
			throw std::out_of_range("Invalid position");
		node_type* toDelete = pos._ptr->_next;
		if (_tail == toDelete) { _tail = pos._ptr; }
		pos._ptr->_next = toDelete->_next;
		delete toDelete;
		--_length;
		return iterator(pos._ptr->_next);
	}
	// 删除索引元素
	void erase(size_type index) {
		if (index >= _length) throw std::out_of_range("Invalid position");
		erase_after(_get_pre_iterator(index));
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
	/*
	this: head		   pos		tail
			|			|		  |
			10->11->..->14->...->18
	1. nTail = tail : new.tail->18
	2. tail = pos : tail->14
	3. nHead = pos.next : new.head->15
	4. pos.next = nullptr : pos->NULL
	*/
	TailLinkedList cut_after(iterator pos) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		TailLinkedList newList;
		if (pos._ptr->_next == nullptr) return newList;
		newList._tail = _tail;
		_tail = pos._ptr;
		newList._sentinel._next = pos._ptr->_next;
		pos._ptr->_next = nullptr;
		// 获取长度
		size_type cutLength = 0;
		node_type* node = newList._sentinel._next;
		while (node != nullptr) {
			++cutLength;
			node = node->_next;
		}
		newList._length = cutLength;
		_length -= cutLength;
		return newList;
	}
	// 在索引和索引上一个元素之间 截断链表并返回截断链表
	TailLinkedList cut(size_type index) {
		if (index >= _length) throw std::out_of_range("Invalid position");
		if (index == 0) {
			TailLinkedList newList;
			newList._sentinel._next = _sentinel._next;
			newList._length = _length;
			newList._tail = _tail;
			_sentinel._next = nullptr;
			_tail = nullptr;
			_length = 0;
			return newList;
		}
		TailLinkedList newList;
		newList._tail = _tail;
		_tail = _get_node_at(index - 1);
		newList._sentinel._next = _tail->_next;
		_tail->_next = nullptr;
		newList._length = _length - index;
		_length = index;
		return newList;
	}

	// 中断为两节 移动衔接其他链表再重链接两节
	/*
	this: head		   pos		tail
			|			|		  |
			10->11->..->14->...->18
	other:head		   tail
			|			 |
			20->21->..->29
	1. 29.next = 14.next : 29->15
	2. 14.next = oHead : 14->20
	3. oHead = nullptr : other.head->NULL
	4. oTail = nullptr : other.tail->NULL
	*/
	TailLinkedList& splice_after(iterator pos, TailLinkedList&& other) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		if (other.empty()) return *this;
		other._tail->_next = pos._ptr->_next;
		pos._ptr->_next = other._sentinel._next;
		other._sentinel._next = nullptr;
		other._tail = nullptr;
		_length += other._length;
		other._length = 0;
		return *this;
	}
	template<typename InputIt>
	TailLinkedList& splice_after(iterator pos, InputIt first, InputIt end) {
		TailLinkedList temp = TailLinkedList(first, end);
		return splice_after(pos, std::move(temp));
	}
	TailLinkedList& splice(size_type index, TailLinkedList&& other) {
		if (index > _length) throw std::out_of_range("Invalid position");
		iterator pos = _get_pre_iterator(index);
		return splice_after(pos, std::move(other));
	}
	TailLinkedList& splice_front(TailLinkedList&& other) {
		return splice_after(before_begin(), std::move(other));
	}
	TailLinkedList& splice_back(TailLinkedList&& other) {
		return splice_after(before_end(), std::move(other));
	}

	bool empty() { return _length == 0; }

	size_type size() const noexcept { return _length; }

private:
	// 获取索引对应指针
	node_type* _get_node_at(size_type index) const {
		if (index >= _length) throw std::out_of_range("Invalid position");
		node_type* node = _sentinel._next;
		for (size_type i = 0; i < index; ++i) {
			node = node->_next;
		}
		return node;
	}

	// 获取索引对应迭代器
	iterator _get_iterator_at(size_type index) {
		if (index >= _length) throw std::out_of_range("Invalid position");
		iterator it = begin();
		for (size_type i = 0; i < index; ++i) { ++it; }
		return it;
	}
	const_iterator _get_iterator_at(size_type index) const {
		if (index >= _length) throw std::out_of_range("Invalid position");
		iterator it = begin();
		for (size_type i = 0; i < index; ++i) { ++it; }
		return it;
	}

	// 获取索引对应前向的迭代器
	iterator _get_pre_iterator(size_type index) {
		if (index > _length) throw std::out_of_range("Invalid position");
		iterator it = before_begin();
		for (size_type i = 0; i < index; ++i) { ++it; }
		return it;
	}
	const_iterator _get_pre_iterator(size_type index) const {
		if (index > _length) throw std::out_of_range("Invalid position");
		iterator it = before_begin();
		for (size_type i = 0; i < index; ++i) { ++it; }
		return it;
	}

public:
	void clear() noexcept {
		node_type* current = _sentinel._next;  // 从真实头节点开始
		while (current != nullptr) {
			node_type* to_delete = current;
			current = current->_next;
			delete to_delete;
		}
		_sentinel._next = nullptr;  // 重置哨兵节点
		_tail = nullptr;           // 重置尾指针
		_length = 0;               // 长度归零
	}

	// 预分配内存（无实际作用，仅为兼容STL接口）
	void reserve(size_type new_cap) { /* 链表无需预留空间 */ }

	// 调整大小
	void resize(size_type count, const value_type& value = value_type()) {
		if (_length > count) {// 缩减大小 - 使用 _clearPtr 一次性截断
			if (count == 0) {
				clear();
			}else {
				node_type* prev = _get_node_at(count - 1);
				_clear_after(prev); // 清除 prev 之后的所有节点
				prev->_next = nullptr;
				_tail = prev;   // 更新尾指针
				_length = count;
			}
			return;
		}
		if (_length < count) {// 增大大小 - 批量插入
			size_type to_add = count - _length;
			TailLinkedList temp(to_add, value); // 创建临时链表
			if (empty()) {
				*this = std::move(temp);   // 直接接管临时链表
			}else {
				splice_back(std::move(temp));
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
		_tail = ptr;
		for (node_type* tempPtr = ptr->_next; ptr != nullptr;) {    
			ptr = tempPtr;          
			tempPtr = ptr->_next; 
			delete ptr;             
			--_length;
		}
		_tail->_next = nullptr;
	}
public:
	friend void swap(TailLinkedList& lhs, TailLinkedList& rhs) noexcept {
		lhs.swap(rhs);
	}

	void swap(TailLinkedList& other) noexcept {
		using std::swap;
		swap(_sentinel, other._sentinel);
		swap(_tail, other._tail);
		swap(_length, other._length);
	}

public:
	// 迭代器相关操作
	iterator begin() noexcept { return iterator(_sentinel._next);}
	iterator before_begin() noexcept { return iterator(&_sentinel); }
	iterator end() noexcept { return iterator(nullptr);}
	iterator before_end() noexcept { return empty() ? end() : iterator(_tail); }

	const_iterator begin() const noexcept { return const_iterator(_sentinel._next); }
	const_iterator before_begin() const noexcept { return const_iterator(&_sentinel); }
	const_iterator end() const noexcept { return const_iterator(nullptr); }
	const_iterator before_end() const noexcept { return empty() ? end() : const_iterator(_tail); }

	const_iterator cbegin() const noexcept { return begin(); }
	const_iterator cbefore_begin() const noexcept {
		return const_iterator(reinterpret_cast<node_type*>(const_cast<SentinelNode*>(&_sentinel)));
	}
	const_iterator cend() const noexcept { return end(); }
	const_iterator cbefore_end() const noexcept { return before_end(); }
};
template<typename T>
template<typename U>
class TailLinkedList<T>::TailLinkedListIterator {
public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = U;
	using difference_type = std::ptrdiff_t;
	using pointer = U*;
	using reference = U&;
	using node_type = TailLinkedList<T>::_Node<T>;
	using iterator = TailLinkedListIterator<T>;
	using const_iterator = TailLinkedListIterator<const T>;
	template<typename> friend class TailLinkedList;
	template<typename> friend class TailLinkedListIterator;

private:
	node_type* _ptr;
public:

	explicit TailLinkedListIterator(node_type* ptr = nullptr) : _ptr(ptr) {}

	// 解引用操作符
	reference operator*() const { return _ptr->_data; }

	pointer operator->() const { return &_ptr->_data; }

	// 前置++
	TailLinkedListIterator& operator++() {
		_ptr = _ptr->_next;
		return *this;
	}

	// 后置++
	TailLinkedListIterator operator++(int) {
		TailLinkedListIterator temp = *this;
		++(*this);
		return temp;
	}

	template<typename OtherU>
	bool operator==(const TailLinkedListIterator<OtherU>& other) const {
		return _ptr == other._ptr;  // 需要_ptr可访问
	}

	template<typename OtherU>
	bool operator!=(const TailLinkedListIterator<OtherU>& other) const {
		return !(*this == other);
	}
private:
	// 获取底层节点指针（供链表类内部使用）
	node_type* _get_node() const { return _ptr; }
};