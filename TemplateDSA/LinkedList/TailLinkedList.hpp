#pragma once
template<typename T>
class TailLinkedList {
private:
	// ǰ��������������
	template<typename U>
	class TailLinkedListIterator;

	// �ڵ��ඨ��
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
	// �ڱ��ڵ㣨����before_begin��
	struct SentinelNode:public _Node<T> {
		SentinelNode() : _Node<T>(T(), nullptr) {}
		SentinelNode(const SentinelNode& other) : _Node<T>(T(), nullptr) {}
		SentinelNode(SentinelNode&& other) : _Node<T>(T(), other._next) {
			other._next = nullptr; // ȷ���ƶ���Դ����Ϊ��
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
	SentinelNode _sentinel = SentinelNode();  // �ڱ��ڵ�
	node_type* _tail = nullptr;       // βָ��
	size_type _length = 0; // ������
public:
	TailLinkedList() noexcept = default;
	// ��乹�캯����n��Ĭ��ֵ��
	explicit TailLinkedList(size_type n) {
		for (size_type i = 0; i < n; ++i) {
			push_back(T()); // Ĭ�Ϲ���Ԫ��
		}
	}

	// ���� n ��value����
	TailLinkedList(size_type n, const T& value) {
		for (size_type i = 0; i < n; ++i) {
			push_back(value); // Ĭ�Ϲ���Ԫ��
		}
	}

	// ��������Χ���캯��
	template<typename InputIt>
	TailLinkedList(InputIt first, InputIt last){
		for (; first != last; ++first) {
			push_back(*first);
		}
	}

	// ��ʼ���б���
	TailLinkedList(std::initializer_list<T> init)
		: TailLinkedList(init.begin(), init.end()) {}; // ί�и����乹�캯��

	// �������캯��
	TailLinkedList(const TailLinkedList& other):TailLinkedList(other.begin(), other.end()) {}

	//�ƶ����캯��
	TailLinkedList(TailLinkedList&& other) noexcept
		: _sentinel(std::move(other._sentinel)),
		_tail(std::move(other._tail)),
		_length(std::move(other._length)) {
		other._sentinel._next = nullptr;
		other._tail = nullptr;
		other._length = 0;
	}

	// ������ֵ����
	TailLinkedList& operator=(const TailLinkedList& other) {
		if (this != &other) {
			TailLinkedList temp(other);
			swap(temp);
		}
		return *this;
	}

	// �ƶ���ֵ����
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

	// ֧�־ۺϳ�ʼ����ֵ
	TailLinkedList& operator=(std::initializer_list<T> init) {
		TailLinkedList temp(init);
		swap(temp);
		return *this;
	}

	~TailLinkedList() {
		clear();
	}
public:
	// ��Ԫ�ط���
	T& front() {
		if (empty()) throw std::out_of_range("LinkedList is empty");
		return _sentinel._next->data; // �ڱ���next����Ԫ��
	}
	const T& front() const noexcept {
		return const_cast<TailLinkedList*>(this)->front(); // ���÷�const�汾
	}

	// βԪ�ط���
	T& back() {
		if (empty()) throw std::out_of_range("LinkedList is empty");
		return _tail->data; // ֱ��ʹ��βָ��
	}
	const T& back() const noexcept {
		return const_cast<TailLinkedList*>(this)->back();
	}

	// ͷ������
	void push_front(const T& value) {
		_sentinel._next = new node_type(value, _sentinel._next);
		if (!_tail) _tail = _sentinel._next; // ԭ����Ϊ��ʱ����βָ��
		++_length;
	}
	void push_front(T&& value) {
		_sentinel._next = new node_type(std::move(value), _sentinel._next);
		if (!_tail) _tail = _sentinel._next;
		++_length;
	}

	// β������
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

	// ͷ��ɾ��
	void pop_front() {
		if (empty()) throw std::out_of_range("LinkedList is empty");
		node_type* to_delete = _sentinel._next;
		_sentinel._next = to_delete->_next;
		if (_tail == to_delete) _tail = nullptr; // ɾ�����һ���ڵ�
		delete to_delete;
		--_length;
	}

	// ��ĳ��Ԫ�غ������Ԫ�� ���ص�����ָ����Ԫ��
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
	// ������λ�� ����Ԫ��
	void insert(size_type index, const T& value) {
		if (index > _length) throw std::out_of_range("Invalid position");
		insert_after(_get_pre_iterator(index), value);
	}
	void insert(size_type index, T&& value) {
		if (index > _length) throw std::out_of_range("Invalid position");
		insert_after(_get_pre_iterator(index), std::move(value));
	}

	// ����������
	template<typename... Args>
	iterator emplace_after(iterator pos, Args&&... args) {
		if (pos == end()) throw std::out_of_range("Invalid position");
		node_type* new_node = new node_type(T(std::forward<Args>(args)...), pos._ptr->_next);
		pos._ptr->_next = new_node;
		++_length;
		return iterator(new_node);
	}
	// ���������첢����Ԫ��
	template<typename... Args>
	void emplace(size_type index, Args&&... args) {
		if (index > _length) throw std::out_of_range("Invalid position");
		emplace_after(_get_pre_iterator(index), std::forward<Args>(args)...);
	}

	// ��ĳ��Ԫ�غ�ɾ��Ԫ�� ���ص�����ָ��ɾ���ĺ�һ��Ԫ��
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
	// ɾ������Ԫ��
	void erase(size_type index) {
		if (index >= _length) throw std::out_of_range("Invalid position");
		erase_after(_get_pre_iterator(index));
		return;
	}

	// ��ȫ����
	T& at(size_type index) {
		if (index >= _length) throw std::out_of_range("Invalid position");
		return _get_node_at(index)->_data;
	}
	const T& at(size_type index) const {
		if (index >= _length) throw std::out_of_range("Invalid position");
		return _get_node_at(index)->_data;
	}

	// �ڵ��������������һ��Ԫ��֮�� �ض��������ؽض�����
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
		// ��ȡ����
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
	// ��������������һ��Ԫ��֮�� �ض��������ؽض�����
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

	// �ж�Ϊ���� �ƶ��ν���������������������
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
	// ��ȡ������Ӧָ��
	node_type* _get_node_at(size_type index) const {
		if (index >= _length) throw std::out_of_range("Invalid position");
		node_type* node = _sentinel._next;
		for (size_type i = 0; i < index; ++i) {
			node = node->_next;
		}
		return node;
	}

	// ��ȡ������Ӧ������
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

	// ��ȡ������Ӧǰ��ĵ�����
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
		node_type* current = _sentinel._next;  // ����ʵͷ�ڵ㿪ʼ
		while (current != nullptr) {
			node_type* to_delete = current;
			current = current->_next;
			delete to_delete;
		}
		_sentinel._next = nullptr;  // �����ڱ��ڵ�
		_tail = nullptr;           // ����βָ��
		_length = 0;               // ���ȹ���
	}

	// Ԥ�����ڴ棨��ʵ�����ã���Ϊ����STL�ӿڣ�
	void reserve(size_type new_cap) { /* ��������Ԥ���ռ� */ }

	// ������С
	void resize(size_type count, const value_type& value = value_type()) {
		if (_length > count) {// ������С - ʹ�� _clearPtr һ���Խض�
			if (count == 0) {
				clear();
			}else {
				node_type* prev = _get_node_at(count - 1);
				_clear_after(prev); // ��� prev ֮������нڵ�
				prev->_next = nullptr;
				_tail = prev;   // ����βָ��
				_length = count;
			}
			return;
		}
		if (_length < count) {// �����С - ��������
			size_type to_add = count - _length;
			TailLinkedList temp(to_add, value); // ������ʱ����
			if (empty()) {
				*this = std::move(temp);   // ֱ�ӽӹ���ʱ����
			}else {
				splice_back(std::move(temp));
			}
		}
	}

private:
	// ���ptr������������
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
	// ��������ز���
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

	// �����ò�����
	reference operator*() const { return _ptr->_data; }

	pointer operator->() const { return &_ptr->_data; }

	// ǰ��++
	TailLinkedListIterator& operator++() {
		_ptr = _ptr->_next;
		return *this;
	}

	// ����++
	TailLinkedListIterator operator++(int) {
		TailLinkedListIterator temp = *this;
		++(*this);
		return temp;
	}

	template<typename OtherU>
	bool operator==(const TailLinkedListIterator<OtherU>& other) const {
		return _ptr == other._ptr;  // ��Ҫ_ptr�ɷ���
	}

	template<typename OtherU>
	bool operator!=(const TailLinkedListIterator<OtherU>& other) const {
		return !(*this == other);
	}
private:
	// ��ȡ�ײ�ڵ�ָ�루���������ڲ�ʹ�ã�
	node_type* _get_node() const { return _ptr; }
};