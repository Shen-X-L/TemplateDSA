#include <functional>
#define TEST_HEAP
template<typename T, typename Compare = std::less<T>>
class Heap {
public:
	using size_type = std::size_t;
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
private:
	T* _ptr;		  // Ԫ������ָ��
	size_type _length;   // Ԫ������
	size_type _capacity; // �����ڴ����� (����2^n-1�ṹ)
	std::function<bool(const T&, const T&)>  _compare;	 // �ȽϺ�������
	static constexpr size_type MIN_CAPACITY = 15; //��С����

public:
	/*
	* ���캯��
	* @param initialCapacity ��ʼ����
	* @param compare �ȽϺ�������
	*/ 
	explicit Heap(size_type initialCapacity = 15, 
				  const Compare& comp = Compare())
		: _ptr(nullptr), _length(0),
		_capacity(initialCapacity > 0 ? get_alloc_size(initialCapacity) : 0), _compare(comp) {
		if (_capacity > 0) {
			_ptr = new T[_capacity];
		}
	}

	/*
	* ��������Χ����
	* @param first ��������ʼλ��
	* @param last ����������λ��
	* @param compare �ȽϺ�������
	*/
	template<typename InputIt>
	Heap(InputIt first, InputIt last, const Compare& comp = Compare())
		: Heap(std::distance(first, last), comp) {
		push_range(first, last);
	}

	/*
	* ��ʼ���б���
	* @param init ��ʼ���б�
	* @param compare �ȽϺ�������
	*/
	Heap(std::initializer_list<T> init, const Compare& comp = Compare())
		: Heap(init.begin(), init.end(), comp) {}  // ί�и����乹�캯��

	/*
	* �������캯��
	* @param other Դ����
	*/
	Heap(const Heap& other)
		: _ptr(new T[other._capacity]), _length(other._length),
		_capacity(other._capacity), _compare(other._compare) {
		std::copy(other._ptr, other._ptr + _length, _ptr);
	}

	/*
	* �ƶ����캯��
	* @param other Դ����
	*/
	Heap(Heap&& other) noexcept
		: Heap() {  // ί��Ĭ�Ϲ���
		swap(*this, other);
	}

	/*
	* ͳһ���������ƶ���ֵ
	* @param other Դ����
	*/
	Heap& operator=(Heap other) {  // ����ʵ�������Զ�ѡ�񿽱�/�ƶ�����
		swap(*this, other);
		return *this;
	}

	/*
    * ֧�־ۺϳ�ʼ����ֵ
    * @param init ��ʼ���б�
    */
	Heap& operator=(std::initializer_list<T> init) {
		Heap temp(init);
		swap(*this, temp);
		return *this;
	}
    /*
    * ʵ�ֽ�������
    * @param first Դ����
    * @param second Ŀ�����
    */
	friend void swap(Heap& first, Heap& second) noexcept {
		using std::swap;
		swap(first._ptr, second._ptr);
		swap(first._length, second._length);
		swap(first._capacity, second._capacity);
		swap(first._compare, second._compare);
	}

	/*
    * ��������
    */ 
	~Heap() { delete[] _ptr; }

	/*
    * @return �Ƿ�Ϊ��
    */ 
	bool empty() const noexcept { return _length == 0; }

	/*
	* @return �Ѵ�С
	*/
	size_type size() const noexcept { return _length; }

    /*
    * @return ������ָ��
    */
    T* data() { return _ptr; }

    /*
    * @return ���ڴ��С
    */
    size_type capacity() const noexcept { return _capacity; }

    /*
    * @return ���Ƿ�������
    */
    bool full() const noexcept { return _length == _capacity; }

	/*
    * ������λԪ��
    * @return ��λԪ��
    */ 
	T& top() {
		if (empty()) throw std::out_of_range("Heap is empty");
		return _ptr[0];
	}
	const T& top() const noexcept {
		if (empty()) throw std::out_of_range("Heap is empty");
		return _ptr[0];
	}

	/*
    * ���Ԫ��
    * @param value ��ӵ�Ԫ��
    */ 
	void push(const T& value) {
		if (_length >= _capacity) _reserve();
		_ptr[_length] = value;
		_heapify_up(_length);
		++_length;
	}
	void push(T&& value) {
		if (_length >= _capacity) _reserve();
		_ptr[_length] = std::move(value);
		_heapify_up(_length);
		++_length;
	}

	/*
    * ��������
    * @param first �����������ʼ������
    * @param last ��������Ľ���������
    */ 
	template<typename InputIt>
	void push_range(InputIt first, InputIt last) {
		size_type count = std::distance(first, last);
		size_type start_index = _length;
		if (_length + count > _capacity) {
			_reserve(_length + count);
		}
		//��������Ԫ��
		for (; first != last; ++first) {
			_ptr[_length++] = *first;
		}

		//����ѡ�񽨶Ѳ���
		if (count == 0) {
			return;
		} else if (count < _length / 4) {
			//����Ԫ�غ��� ���_heapify_up����Ч
			for (; start_index < _length; ++start_index) {
				_heapify_up(start_index);
			}
		} else {
			//����Ԫ�ؽ϶� ���彨�Ѹ���Ч
			_rebuild_heap();
		}
	}

	/*
    * �ƻ���, ɾ���Ѷ�Ԫ��
    */ 
	void pop() {
		if (empty()) throw std::out_of_range("Heap is empty");
		std::swap(_ptr[0], _ptr[_length - 1]);
		--_length;
		_heapify_down(0);
		_ensure_length();
	}

	/*
	* @brief �ƻ���, ����ղ��ֶ�����
	* @brief ���Ѷ�Ԫ�ص����������������
	* @param first ��ʼ���������
	* @param last �������������
	*/
	template<typename OutputIt>
	void pop_range(OutputIt first, OutputIt last) {
		while (first != last && !empty()) {
			*first++ = std::move(top());
			pop();
		}
	}

	/*
	* @brief �ƻ���, �����ȫ��������
	* @brief ������������������� 
	* @param dest ��ʼ���������
	*/
	template<typename OutputIt>
	void sort_to(OutputIt dest) {
		while (!empty()) {
			*dest++ = std::move(top());
			pop();
		}
	}
	/*
    * ��ն�
    */ 
	void clear() {
		_length = 0;
		_ensure_length();
	}

	/*
	* �Ѽ��
	* @return �������Ƿ�����
	*/
	bool is_valid_heap() const {
		for (size_type i = 0; i < _length; ++i) {
			size_type left = 2 * i + 1;
			size_type right = 2 * i + 2;

			if (left < _length && _compare(_ptr[left], _ptr[i])) {
				return false;  // �ӽڵ�С�ڸ��ڵ�
			}
			if (right < _length && _compare(_ptr[right], _ptr[i])) {
				return false;
			}
		}
		return true;
	}

	/*
    * �ı�ȽϺ���
    * @param newCompare �µıȽϺ���
    */
	template<typename NewCompare>
	void change_compare(NewCompare newCompare) {
		_compare = std::function<bool(const T&, const T&)>(newCompare);
		_rebuild_heap();
	}

	/*
	* �ı�����
	* @param new_capacity �µ�����
	*/
	void reserve(size_type new_capacity) {
		if (new_capacity > _capacity) {
			_reserve(new_capacity);
		}
	}

	/*
    * ����Ԫ�� O(n)
    * @param value Ҫ���ҵ�Ԫ��
    * @return �ҵ���Ԫ�ص�ָ��
    */
	template<typename U>
	T* find(const U& value) {
		for (size_type i = 0; i < _length; ++i) {
			if (_ptr[i] == value) {
				return &_ptr[i];
			}
		}
		return nullptr;
	}

	/*
    * ����Ԫ��ָ��ĸ���
    * @param pos Ҫ���µ�Ԫ��ָ��
    * @param new_value �µ�Ԫ��
    * @return Ԫ��ָ���Ƿ�Խ��
    */ 
	bool update_at(T* pos, const T& new_value) {
		if (pos < _ptr || pos >= _ptr + _length) {
			return false;  // ָ��Խ����
		}

		size_type index = static_cast<size_type>(pos - _ptr);
		_ptr[index] = new_value;

		// ���¶ѻ�
		_heapify_up(index);
		_heapify_down(index);
		return true;
	}
    /*
    * ����Ԫ��ָ��ĸ���
    * @param pos Ҫ���µ�Ԫ��ָ��
    * @param new_value �µ�Ԫ��
    * @return Ԫ��ָ���Ƿ�Խ��
    */
	bool update_at(const T* pos, const T& new_value) {
		return update_at(const_cast<T*>(pos), new_value);
	}

	/*
    * �����ڴ浽���ʴ�С
    */
	void shrink_to_fit() {
		if (_length == 0) {
			clear();
			return;
		}

		size_type new_capacity = get_alloc_size(_length);
		if (new_capacity < _capacity) {
			T* new_ptr = new T[new_capacity];
			for (size_type i = 0; i < _length; ++i) {
				new_ptr[i] = std::move(_ptr[i]);
			}
			delete[] _ptr;
			_ptr = new_ptr;
			_capacity = new_capacity;
		}
	}

	/*
    * �ϲ�������
    * @param other ��һ���� (�ƶ���)
    */
	void merge(Heap&& other) {
		push_range(other.begin(), other.end());
		other.clear();
	}

	//���������
	T* begin() noexcept { return _ptr; }
	const T* begin() const noexcept { return _ptr; }
	T* end() noexcept { return _ptr + _length; }
	const T* end() const noexcept { return _ptr + _length; }
	// const�汾
	const T* cbegin() const noexcept { return _ptr; }
	const T* cend() const noexcept { return _ptr + _length; }
	// ��ӷ��������֧��
	std::reverse_iterator<T*> rbegin() noexcept {
		return std::reverse_iterator<T*>(end());
	}
	const std::reverse_iterator<const T*> rbegin() const noexcept {
		return std::reverse_iterator<const T*>(end());
	}
	std::reverse_iterator<T*> rend() noexcept {
		return std::reverse_iterator<T*>(begin());
	}
	const std::reverse_iterator<const T*> rend() const noexcept {
		return std::reverse_iterator<const T*>(begin());
	}

	/*
	* @brief ��ȡ Ӧ�÷�����ڴ��С
	* @param num �ڴ��С
	* @return ����num��С���� max(2^n-1,15)
	*/
	static size_type get_alloc_size(size_type num) {
		if (num == 0) return 0;
		if (num <= 15) return 15;
		return supremum_mask(num);
	}

	/*
	* @param num ������
	* @return ���� num ��2^n-1 ���½�
	*/
	static size_type supremum_mask(size_type num) {
		num |= num >> 1;
		num |= num >> 2;
		num |= num >> 4;
		num |= num >> 8;
		num |= num >> 16;
		// �����64λ���ͣ���Ҫ�������λ
		if constexpr (sizeof(size_type) > 4) {
			num |= num >> 32;
		}
		return num;
	}
private:
	/*
    * �ϸ�������
    * @param index �������Ľڵ�����
    */
	void _heapify_up(size_type index) {
		while (index > 0) {
			size_type parent = (index - 1) / 2;
			if (_compare(_ptr[index], _ptr[parent])) {
				std::swap(_ptr[index], _ptr[parent]);
				index = parent;
			}
			else {
				break;
			}
		}
	}

	/*
    * �³�������
    * @param index �������Ľڵ�����
    */
	void _heapify_down(size_type index) {
		while (true) {
			size_type left = 2 * index + 1;
			size_type right = 2 * index + 2;
			size_type smallest = index;
			if (left < _length && _compare(_ptr[left], _ptr[smallest])) {
				smallest = left;
			}
			if (right < _length && _compare(_ptr[right], _ptr[smallest])) {
				smallest = right;
			}
			if (smallest != index) {
				std::swap(_ptr[index], _ptr[smallest]);
				index = smallest;
			}
			else {
				break;
			}
		}
	}

	/*
    * @brief ��չ�ڴ�
    * @brief �ڴ�Ϊmax(2^n-1,15)
    * @param minCapacity ��С����
    */ 
	void _reserve(size_type minCapacity = 0) {
		size_type newCapacity = (_capacity == 0) ? MIN_CAPACITY : _capacity * 2 + 1;
		if (minCapacity > newCapacity) {
			newCapacity = get_alloc_size(minCapacity);
		}
		T* newPtr = new T[newCapacity];
		for (size_type i = 0; i < _length; ++i) {
			newPtr[i] = std::move(_ptr[i]);
		}
		delete[] _ptr;
		_ptr = newPtr;
		_capacity = newCapacity;
	}

	/*
    * �����ڴ�, ����*4+3 > �ڴ����� ʱ����
    */ 
	void _ensure_length() {
		if (_capacity  < MIN_CAPACITY || ((_length + 1) * 4) > _capacity + 1) return;
		size_type newCapacity = std::max(MIN_CAPACITY, (_capacity + 1) / 2 - 1);
		if (newCapacity < _length) return;
		T* newPtr = new T[newCapacity];
		for (size_type i = 0; i < _length; ++i) {
			newPtr[i] = std::move(_ptr[i]);
		}
		delete[] _ptr;
		_ptr = newPtr;
		_capacity = newCapacity;
	}

	/*
    * Floyd ���� ���¹�����
    */  
	void _rebuild_heap() {
		if (_length <= 1) return;
		size_type i = (_length - 1) / 2;
		while (true) { 
			// ѭ�������һ����Ҷ�ӽڵ㿪ʼ���Ե����Ͻ��жѻ�
			_heapify_down(i);
			if (i == 0) break;
			--i;
		}
	}
};

void test_heap_operations() {
	std::cout << "=== ���Ի����Ѳ��� ===" << std::endl;

   //����Ĭ�Ϲ���
	Heap<int> heap1;
	assert(heap1.empty());
	assert(heap1.size() == 0);

   //���Բ����top
	heap1.push(5);
	heap1.push(2);
	heap1.push(8);
	heap1.push(1);

	assert(heap1.size() == 4);
	assert(heap1.top() == 1);  // ��С�ѣ�Ӧ������Сֵ

   //����pop
	heap1.pop();
	assert(heap1.top() == 2);
	heap1.pop();
	assert(heap1.top() == 5);

	std::cout << "�����Ѳ�������ͨ��" << std::endl;
}

void test_initialization() {
	std::cout << "\n=== ���Գ�ʼ�� ===" << std::endl;

   //���Գ�ʼ���б�
	Heap<int> heap2{ 3, 1, 4, 1, 5, 9, 2 };
	assert(heap2.top() == 1);

   //���Ե���������
	std::vector<int> vec{ 6, 3, 8, 2, 9 };
	Heap<int> heap3(vec.begin(), vec.end());
	assert(heap3.top() == 2);

   //���Կ�������
	Heap<int> heap4 = heap3;
	assert(heap4.top() == 2);
	heap3.pop();
	assert(heap4.top() == 2);  // ����Ӧ�ö���

   //�����ƶ�����
	Heap<int> heap5 = std::move(heap4);
	assert(heap5.top() == 2);
	assert(heap4.empty());  // �ƶ���ԭ����ӦΪ��

	std::cout << "��ʼ������ͨ��" << std::endl;
}

void test_capacity_management() {
	std::cout << "\n=== ������������ ===" << std::endl;

	Heap<int> heap;
	size_t initial_capacity = heap.capacity();
	std::cout << "��ʼ����: " << initial_capacity << std::endl;

   //��������
	for (int i = 0; i < 20; ++i) {
		heap.push(i);
	}
	std::cout << "����20��Ԫ�غ�����: " << heap.capacity() << std::endl;
	assert(heap.capacity() > initial_capacity);

   //��������
	for (int i = 0; i < 15; ++i) {
		heap.pop();
	}
	std::cout << "ɾ��15��Ԫ�غ�����: " << heap.capacity() << std::endl;

	std::cout << "�����������ͨ��" << std::endl;
}

void test_custom_comparator() {
	std::cout << "\n=== �����Զ���Ƚ��� ===" << std::endl;

	//����
	Heap<int, std::greater<int>> max_heap;
	max_heap.push(1);
	max_heap.push(3);
	max_heap.push(2);

	assert(max_heap.top() == 3);  // ���ѣ�Ӧ�������ֵ

   //���Ը��ıȽ���
	Heap<int> min_heap{ 5, 2, 8 };
	assert(min_heap.top() == 2);

	min_heap.change_compare(std::greater<int>());
	assert(min_heap.top() == 8);  // ����Ӧ�������ֵ

	std::cout << "�Զ���Ƚ�������ͨ��" << std::endl;
}

void test_edge_cases() {
	std::cout << "\n=== ���Ա߽���� ===" << std::endl;

   //���Կն�
	Heap<int> empty_heap;
	try {
		empty_heap.top();
		assert(false);  // Ӧ���׳��쳣
	} catch (const std::out_of_range&) {
		std::cout << "�ն�top()�쳣����ͨ��" << std::endl;
	}

	try {
		empty_heap.pop();
		assert(false);  // Ӧ���׳��쳣
	} catch (const std::out_of_range&) {
		std::cout << "�ն�pop()�쳣����ͨ��" << std::endl;
	}

   //���Ե���Ԫ��
	Heap<int> single_heap;
	single_heap.push(42);
	assert(single_heap.top() == 42);
	single_heap.pop();
	assert(single_heap.empty());

   //�����ظ�Ԫ��
	Heap<int> dup_heap{ 2, 2, 2, 1, 1, 3 };
	assert(dup_heap.top() == 1);
	dup_heap.pop();
	assert(dup_heap.top() == 1);

	std::cout << "�߽��������ͨ��" << std::endl;
}

void test_performance() {
	std::cout << "\n=== �������� ===" << std::endl;

	const int N = 1000;
	Heap<int> heap;

	//�����������
	std::vector<int> numbers;
	for (int i = 0; i < N; ++i) {
		numbers.push_back(i);
	}

	std::shuffle(numbers.begin(), numbers.end(),
				 std::mt19937{ std::random_device{}() });

	auto start = std::chrono::high_resolution_clock::now();

	for (int num : numbers) {
		heap.push(num);
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "���� " << N << " ��Ԫ�غ�ʱ: " << duration.count() << " ��s" << std::endl;

	//��֤������
	int last = heap.top();
	while (!heap.empty()) {
		int current = heap.top();
		heap.pop();
		assert(current >= last);  // ������С�ѣ�������Ԫ��Ӧ�÷ǵݼ�
		last = current;
	}

	std::cout << "���ܲ���ͨ��" << std::endl;
}

void test_iterators() {
	std::cout << "\n=== ���Ե����� ===" << std::endl;

	Heap<int> heap{ 3, 1, 4, 1, 5 };

   //���Է�Χforѭ��
	std::cout << "����Ԫ��: ";
	for (const auto& elem : heap) {
		std::cout << elem << " ";
	}
	std::cout << std::endl;

   //���Ա�׼�㷨
	auto it = std::find(heap.begin(), heap.end(), 4);
	assert(it != heap.end());
	assert(*it == 4);

	std::cout << "����������ͨ��" << std::endl;
}
