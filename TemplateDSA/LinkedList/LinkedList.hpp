#pragma once
#include <functional>
#include <stdexcept>        //std::out_of_range
#include <initializer_list> //std::initializer_list<>
#define LINKEDLIST

#ifdef TEST
#include <iostream>
#include <cassert>
#include <vector>
#endif

//封装_单向_链表
template<typename T>
class LinkedList {
private:
	// 声明迭代器类
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
		explicit _Node(const U& value, node_type* _ptr = nullptr)
			: _data(value), _next(_ptr) {}
		explicit _Node(U&& value, node_type* _ptr = nullptr)
			: _data(std::move(value)), _next(_ptr) {}
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

	/*
	* 构造 initialSize 个Type默认构造对象
	* @param initialSize 初始化长度
	* @param value 默认构造对象
	*/
	explicit LinkedList(size_type initialSize, const T& value = T())
		: _head(nullptr), _length(initialSize) {
		/*
		* size=length=3 head=0->1->2
		* i=1 next=1 prev=0->1 prev=1 ++i
		* i=2 next=2 prev=1->2 prev=2 ++i
		* i=3 end
		*/
		if (_length == 0) { return; }
		_head = new node_type(value);
		node_type* prev = _head;
		for (size_type i = 1; i < _length; ++i) {
			prev->_next = new node_type(value);
			prev = prev->_next;
		}
	}

	/*
	* 范围迭代器构造函数
	* @param first 起始迭代器
	* @param last 结束迭代器
	* @tparam InputIt 迭代器类型 通过SFINAE限制类型
	*/ 
	template<typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
	LinkedList(InputIt first, InputIt last)
		: _head(nullptr), _length(0) {

		if (first == last) return;
		_head = new node_type(*first);
		node_type* prev = _head;
		_length = 1;

		for (++first; first != last; ++first) {
			prev->_next = new node_type(*first);
			prev = prev->_next;
			++_length;
		}
	}

	/*
	* 初始化列表构造
	* @param init 初始化列表
	*/
	LinkedList(std::initializer_list<T> init)
		: LinkedList(init.begin(), init.end()) {}; // 委托给范围迭代器构造函数

	/*
	* 拷贝构造函数
	* @param other 待拷贝的链表
	*/ 
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

	/*
	* 移动构造函数
	* @param other 待移动的链表
	*/
	LinkedList(LinkedList&& other)noexcept
		: _head(other._head), _length(other._length) {
		other._head = nullptr;
		other._length = 0;
	}

	/*
	* 统一处理拷贝和移动赋值
	* @param other 源对象（根据参数类型自动选择拷贝/移动构造）
	*/
	LinkedList& operator=(LinkedList other) noexcept {
		swap(*this, other);
		return *this;
	}

	/*
	* 支持聚合初始化赋值
	* @param init 初始化列表
	*/
	LinkedList& operator=(std::initializer_list<T> init) {
		LinkedList temp(init);
		swap(*this, temp);
		return *this;
	}
	/*
	* 友元swap函数，直接交换所有成员
	*/
	friend void swap(LinkedList& lhs, LinkedList& rhs) noexcept {
		using std::swap;
		swap(lhs._head, rhs._head);
		swap(lhs._length, rhs._length);
	}

	/*
	* 析构函数
	*/
	~LinkedList() {
		clear();
	}

//主结构操作函数
public:

	/*
	* 返回首位元素
	* @return 首位元素引用
	* @throw std::out_of_range 链表为空
	*/
	T& front() {
		if (empty()) 
			throw std::out_of_range("LinkedList is empty LinkedList::front");
		return _head->_data;
	}
	const T& front() const noexcept {
		if (empty()) 
			throw std::out_of_range("LinkedList is empty LinkedList::front");
		return _head->_data;
	}

	/*
	* 添加首位元素
	* @param value 待添加元素
	*/ 
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

	/*
	* 构造首位元素
	* @param args 构造参数
	*/ 
	template<typename... Args>
	void emplace_front(Args&&... args) {
		_head = new node_type(T(std::forward<Args>(args)...), _head);
		++_length;
	}
	
	/*
	* 移除首位元素
	* @throw std::out_of_range 链表为空
	*/ 
	void pop_front() {
		if (empty()) 
			throw std::out_of_range("LinkedList is empty LinkedList::pop_front");
		node_type* toDelete = _head;
		_head = _head->_next;
		delete toDelete;
		--_length;
	}

	/*
	* 在迭代器后一位插入新元素, 返回迭代器指向新元素
	* @param pos 在该迭代器之后位置插入
	* @param value 待插入元素
	* @return 迭代器指向新元素
	* @throw std::out_of_range 迭代器越界
	*/
	iterator insert_after(iterator pos, const T& value) {
		if (pos == end()) 
			throw std::out_of_range("Index out of range LinkedList::insert_after");
		node_type* new_node = new node_type(value, pos._ptr->_next);
		pos._ptr->_next = new_node;
		++_length;
		return iterator(new_node);
	}
	iterator insert_after(iterator pos, T&& value) {
		if (pos == end()) 
			throw std::out_of_range("Index out of range LinkedList::insert_after");
		node_type* new_node = new node_type(std::move(value), pos._ptr->_next);
		pos._ptr->_next = new_node;
		++_length;
		return iterator(new_node);
	}

	/*
	* 在索引位置插入元素, 原本索引位置元素向后偏移一位
	* @param index 在该索引位置插入
	* @param value 待插入元素
	* @throw std::out_of_range 索引越界
	*/ 
	void insert(size_type index, const T& value) {
		if (index > _length) 
			throw std::out_of_range("Index out of range LinkedList::insert");
		if (index == 0) {
			push_front(std::move(value));
			return;
		}
		node_type* prev = _get_node_at(index - 1);
		insert_after(iterator(prev), value);
	}
	void insert(size_type index, T&& value) {
		if (index > _length) 
			throw std::out_of_range("Index out of range LinkedList::insert");
		if (index == 0) {
			push_front(std::move(value));
			return;
		}
		node_type* prev = _get_node_at(index - 1);
		insert_after(iterator(prev), std::move(value));
	}

	/*
	* 在迭代器后一位构造并插入元素, 返回迭代器指向新元素
	* @param pos 在该迭代器之后位置插入
	* @param args 构造参数
	* @return 迭代器指向新元素
	* @throw std::out_of_range 迭代器越界
	*/
	template<typename... Args>
	iterator emplace_after(iterator pos, Args&&... args) {
		if (pos == end()) 
			throw std::out_of_range("Index out of range LinkedList::emplace_after");
		node_type* new_node = new node_type(T(std::forward<Args>(args)...), pos._ptr->_next);
		pos._ptr->_next = new_node;
		++_length;
		return iterator(new_node);
	}

	/*
	* 在索引位置插入元素,  原本索引位置元素向后偏移一位
	* @param index 在该索引位置插入
	* @param args 构造参数
	* @throw std::out_of_range 索引越界
	*/
	template<typename... Args>
	void emplace(size_type index, Args&&... args) {
		if (index > _length) 
			throw std::out_of_range("Index out of range LinkedList::emplace");
		if (index == 0) {
			emplace_front(std::forward<Args>(args)...);
			return;
		}
		node_type* prev = _get_node_at(index - 1);
		emplace_after(iterator(prev), std::forward<Args>(args)...);
	}

	/*
	* 删除迭代器后一位元素, 返回迭代器指向被删除元素的后一个迭代器
	* @param pos 在该迭代器之后位置删除
	* @return 迭代器指向被删除元素的后一个迭代器
	* @throw std::out_of_range 迭代器越界
	*/ 
	iterator erase_after(iterator pos) {
		if (pos == end() || pos._ptr->_next == nullptr)
			throw std::out_of_range("Index out of range LinkedList::erase_after");
		node_type* toDelete = pos._ptr->_next;
		pos._ptr->_next = toDelete->_next;
		delete toDelete;
		--_length;
		return iterator(pos._ptr->_next);
	}

	/*
	* 删除索引元素
	* @param index 索引位置
	* @throw std::out_of_range 索引越界
	*/ 
	void erase(size_type index) {
		if (index >= _length) 
			throw std::out_of_range("Index out of range LinkedList::erase");
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

	/*
	* 安全访问索引位置元素
	* @param index 索引
	* @return 索引位置元素引用
	* @throw std::out_of_range 索引越界
	*/ 
	T& at(size_type index) {
		if (index >= _length) throw std::out_of_range("Index out of range LinkedList::at");
		return _get_node_at(index)->_data;
	}
	const T& at(size_type index) const {
		if (index >= _length) throw std::out_of_range("Index out of range LinkedList::at");
		return _get_node_at(index)->_data;
	}

	/*
	* 截断迭代器与下一个元素连接, 并返回截断链表
	* @param pos 迭代器
	* @return 截断链表
	* @throw std::out_of_range 迭代器越界
	*/ 
	LinkedList cut_after(iterator pos) {
		if (pos == end()) throw std::out_of_range("Index out of range LinkedList::cut_after");
		LinkedList newList;
		if (pos._ptr->_next == nullptr) return newList;
		newList._head = pos._ptr->_next;
		pos._ptr->_next = nullptr;
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

	/*
	* 截断索引和上一个元素连接, 并返回截断链表
	* @param index 索引位置
	* @return 截断链表
	* @throw std::out_of_range 索引越界
	*/
	LinkedList cut(size_type index) {
		if (index >= _length) throw std::out_of_range("Index out of range LinkedList::cut");
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

	/*
	* @brief 在迭代器与下一个元素之间, 中断为两节 
	* @brief 移动衔接其他链表再重链接两节
	* @param pos 迭代器
	* @param other 待连接链表 (移动操作)
	* @return *this 链表引用
	* @throw std::out_of_range 迭代器越界
	*/ 
	LinkedList& splice_after(iterator pos, LinkedList&& other) {
		/*
		* this = a->b->c->d pos = b other = e->f->g
		* a->b c->d e->f->g
		* a->b->e->f->g c->d
		* a->b->e->f->g->c->d
		*/
		if (pos == end()) throw std::out_of_range("Index out of range LinkedList::splice_after");
		if (other.empty()) return *this;
		node_type* oldNext = pos._ptr->_next;
		pos._ptr->_next = other._head; //链表1断开3, 重链接2
		node_type* tail = other._head; //尾节点
		while (tail->_next != nullptr) {
			tail = tail->_next;
		}
		tail->_next = oldNext; //链表2重链接3
		_length += other._length;
		other._head = nullptr;
		other._length = 0;
		return *this;
	}

	/*
	* @brief 在迭代器与下一个元素之间, 中断为两节
	* @brief 移动衔接其他链表再重链接两节
	* @param pos 迭代器
	* @param first 输入迭代器开始位置 (拷贝操作)
	* @param last 输入迭代器结束位置 (拷贝操作)
	* @return *this 链表引用
	* @throw std::out_of_range 迭代器越界
	*/
	template<typename InputIt>
	LinkedList& splice_after(iterator pos, InputIt first, InputIt end) {
		LinkedList temp = LinkedList(first, end);
		return splice_after(pos, std::move(temp));
	}

	/*
	* @brief 在索引和索引上一个元素之间, 中断为两节
	* @brief 移动衔接其他链表再重链接两节
	* @param index 索引位置
	* @param other 待连接链表 (移动操作)
	* @return *this 链表引用
	* @throw std::out_of_range 索引越界
	*/
	LinkedList& splice(size_type index, LinkedList&& other) {
		if (index > _length) throw std::out_of_range("Index out of range LinkedList::splice");
		if (index == 0) {
			return splice_front(std::move(other));
		}
		node_type* prev = _get_node_at(index - 1);
		return splice_after(iterator(prev), std::move(other));
	}

	/*
	* 将其他链表拼接到当前链表头部
	* @param other 待连接链表 (移动操作)
	* @return *this 链表引用
	*/
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

private:
	/*
	* 获取索引对应指针
	* @param index 索引位置
	* @return 对应指针
	*/
	node_type* _get_node_at(size_type index) const {
		if (index >= _length) 
			throw std::out_of_range("Index out of range LinkedList::_get_node_at");
		node_type* node = _head;
		for (size_type i = 0; i < index; ++i) {
			node = node->_next;
		}
		return node;
	}

//内存操作函数
public:
	/*
	* @return 链表是否为空
	*/ 
	bool empty() const noexcept { return _length == 0; }

	/*
	* @return 链表长度
	*/
	size_type size() const noexcept { return _length; }

	/*
	* 清空全链表
	*/
	void clear() {
		node_type* temp;
		while (_head) {
			temp = _head;
			_head = _head->_next;
			delete temp;
		}
		_length = 0;
	}

	/*
	* 预分配内存（无实际作用，仅为兼容STL接口）
	*/ 
	void reserve(size_type new_cap) { /* 链表无需预留空间 */ }

	/*
	* 改变链表长度, 小于旧长度则截断, 大于旧长度则批量插入
	* @param count 新长度
	* @param value 默认填充值
	*/ 
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

	/*
	* 批量删除节点
	* @param prev 删除该节点之后的所有节点
	*/
	void _clear_after(node_type* prev) {
		if (!prev) return;

		node_type* current = prev->_next;
		while (current != nullptr) {
			node_type* next = current->_next;
			delete current;
			current = next;
			--_length;
		}
		prev->_next = nullptr;
	}

// 迭代器相关
public:
	
	iterator begin() noexcept { return iterator(_head); }
	iterator end() noexcept { return iterator(nullptr); }
	iterator before_end() noexcept {
		return empty() ? end() : iterator(_get_node_at(_length - 1));
	}
	const_iterator begin() const noexcept { return const_iterator(_head); }
	const_iterator end() const noexcept { return const_iterator(nullptr); }
	const_iterator before_end() const noexcept {
		return empty() ? end() : const_iterator(_get_node_at(_length - 1));
	}
	const_iterator cbegin() const noexcept { return begin(); }
	const_iterator cend() const noexcept { return end(); }
	const_iterator cbefore_end() const noexcept {return before_end();}
};
//迭代器类
template<typename T>
template<typename U>
class LinkedList<T>::LinkedListIterator {
public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = U;
	using difference_type = std::ptrdiff_t;
	using pointer = U*;
	using reference = U&;
	using node_type = LinkedList<T>::_Node<T>;
	using iterator = LinkedListIterator<T>;
	using const_iterator = LinkedListIterator<const T>;
	template<typename> friend class LinkedList;
	template<typename> friend class LinkedListIterator;
private:
	node_type* _ptr;// 指向链表节点的指针
public:
	explicit LinkedListIterator(node_type* _ptr) : _ptr(_ptr) {}

	reference operator*() const { return _ptr->_data; }
	pointer operator->() const { return &_ptr->_data; }

	LinkedListIterator& operator++() {
		_ptr = _ptr->_next;
		return *this;
	}

	LinkedListIterator operator++(int) {
		LinkedListIterator tmp = *this;
		++(*this);
		return tmp;
	}

	template<typename OtherU>
	bool operator==(const LinkedListIterator<OtherU>& other) const {
		return _ptr == other._ptr;  // 需要_ptr可访问
	}

	template<typename OtherU>
	bool operator!=(const LinkedListIterator<OtherU>& other) const {
		return !(*this == other);
	}

	operator LinkedListIterator<const U>() const {
		return LinkedListIterator<const U>(_ptr);
	}
private:
	node_type* node() const { return _ptr; }
};

#ifdef TEST

void test_basic_operations() {
	std::cout << "=== 测试基本操作 ===" << std::endl;

	LinkedList<int> list;
	assert(list.empty());
	assert(list.size() == 0);

	// 测试push_front
	list.push_front(3);
	list.push_front(2);
	list.push_front(1);
	assert(list.size() == 3);
	assert(list.front() == 1);

	// 测试pop_front
	list.pop_front();
	assert(list.front() == 2);
	assert(list.size() == 2);

	std::cout << "基本操作测试通过 " << std::endl;
}

void test_initialization() {
	std::cout << "\n=== 测试初始化 ===" << std::endl;

	// 测试大小+值构造
	LinkedList<int> list1(3, 42);
	assert(list1.size() == 3);
	for (const auto& elem : list1) {
		assert(elem == 42);
	}

	// 测试初始化列表
	LinkedList<int> list2{ 1, 2, 3, 4, 5 };
	assert(list2.size() == 5);
	assert(list2.front() == 1);

	// 测试迭代器构造
	std::vector<int> vec{ 6, 7, 8 };
	LinkedList<int> list3(vec.begin(), vec.end());
	assert(list3.size() == 3);

	std::cout << "初始化测试通过 " << std::endl;
}

void test_insert_erase() {
	std::cout << "\n=== 测试插入删除 ===" << std::endl;

	LinkedList<int> list{ 1, 2, 3 };

	// 测试insert
	list.insert(1, 99);  // 在索引1插入
	assert(list.size() == 4);

	auto it = list.begin();
	++it;  // 指向第二个元素
	assert(*it == 99);

	// 测试erase
	list.erase(1);  // 删除索引1的元素
	assert(list.size() == 3);

	it = list.begin();
	++it;
	assert(*it == 2);  // 现在第二个元素是2

	std::cout << "插入删除测试通过 " << std::endl;
}

void test_copy_move() {
	std::cout << "\n=== 测试拷贝移动 ===" << std::endl;

	LinkedList<int> original{ 1, 2, 3 };

	// 测试拷贝构造
	LinkedList<int> copy = original;
	assert(copy.size() == 3);
	assert(original.size() == 3);  // 原对象不变

	// 测试移动构造
	LinkedList<int> moved = std::move(original);
	assert(moved.size() == 3);
	assert(original.empty());  // 原对象被移动

	// 测试拷贝赋值
	LinkedList<int> assigned;
	assigned = copy;
	assert(assigned.size() == 3);

	// 测试移动赋值
	LinkedList<int> target;
	target = std::move(copy);
	assert(target.size() == 3);
	assert(copy.empty());

	std::cout << "拷贝移动测试通过 " << std::endl;
}

void test_splice_cut() {
	std::cout << "\n=== 测试拼接截断 ===" << std::endl;

	LinkedList<int> list1{ 1, 2, 3 };
	LinkedList<int> list2{ 4, 5, 6 };

	// 测试splice_after
	auto it = list1.begin();
	++it;  // 指向第二个元素
	list1.splice_after(it, std::move(list2));

	assert(list1.size() == 6);
	assert(list2.empty());

	// 验证顺序: 1->2->4->5->6->3
	std::vector<int> expected{ 1, 2, 4, 5, 6, 3 };
	size_t i = 0;
	for (const auto& elem : list1) {
		assert(elem == expected[i++]);
	}

	// 测试cut
	LinkedList<int> cut_list = list1.cut(3);  // 从索引3处截断
	assert(list1.size() == 3);
	assert(cut_list.size() == 3);

	std::cout << "拼接截断测试通过 " << std::endl;
}

void test_iterators() {
	std::cout << "\n=== 测试迭代器 ===" << std::endl;

	LinkedList<int> list{ 1, 2, 3, 4, 5 };

	// 测试正向迭代
	int sum = 0;
	for (const auto& elem : list) {
		sum += elem;
	}
	assert(sum == 15);

	// 测试STL算法兼容性
	auto it = std::find(list.begin(), list.end(), 3);
	assert(it != list.end());
	assert(*it == 3);

	// 测试const迭代器
	const LinkedList<int>& const_list = list;
	auto cit = const_list.begin();
	assert(*cit == 1);

	std::cout << "迭代器测试通过 " << std::endl;
}

void test_edge_cases() {
	std::cout << "\n=== 测试边界情况 ===" << std::endl;

	// 测试空链表
	LinkedList<int> empty_list;
	try {
		empty_list.front();
		assert(false);  // 应该抛出异常
	} catch (const std::out_of_range&) {
		std::cout << "空链表front()异常测试通过 " << std::endl;
	}

	try {
		empty_list.pop_front();
		assert(false);  // 应该抛出异常
	} catch (const std::out_of_range&) {
		std::cout << "空链表pop_front()异常测试通过 " << std::endl;
	}

	// 测试单个元素
	LinkedList<int> single{ 42 };
	assert(single.front() == 42);
	single.pop_front();
	assert(single.empty());

	std::cout << "边界情况测试通过 " << std::endl;
}

void test_resize() {
	std::cout << "\n=== 测试resize ===" << std::endl;

	LinkedList<int> list{ 1, 2, 3 };

	// 测试缩小
	list.resize(2);
	assert(list.size() == 2);

	// 测试扩大
	list.resize(5, 99);
	assert(list.size() == 5);

	// 验证新元素值
	auto it = list.begin();
	for (int i = 0; i < 2; ++i) ++it;  // 跳过前两个原始元素
	for (int i = 0; i < 3; ++i) {
		assert(*it == 99);
		++it;
	}

	std::cout << "resize测试通过 " << std::endl;
}

int test_linked_list() {
	try {
		test_basic_operations();
		test_initialization();
		test_insert_erase();
		test_copy_move();
		test_splice_cut();
		test_iterators();
		test_edge_cases();
		test_resize();

		std::cout << "\n所有测试通过！链表实现正确。" << std::endl;
		return 0;
	} catch (const std::exception& e) {
		std::cerr << "测试失败: " << e.what() << std::endl;
		return 1;
	}
}
#endif