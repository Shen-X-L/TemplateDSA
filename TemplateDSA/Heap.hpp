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
	T* _ptr;		  // 元素数组指针
	size_type _length;   // 元素数量
	size_type _capacity; // 分配内存容量 (保持2^n-1结构)
	std::function<bool(const T&, const T&)>  _compare;	 // 比较函数对象
	static constexpr size_type MIN_CAPACITY = 15; //最小容量

public:
	/*
	* 构造函数
	* @param initialCapacity 初始容量
	* @param compare 比较函数对象
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
	* 迭代器范围构造
	* @param first 迭代器起始位置
	* @param last 迭代器结束位置
	* @param compare 比较函数对象
	*/
	template<typename InputIt>
	Heap(InputIt first, InputIt last, const Compare& comp = Compare())
		: Heap(std::distance(first, last), comp) {
		push_range(first, last);
	}

	/*
	* 初始化列表构造
	* @param init 初始化列表
	* @param compare 比较函数对象
	*/
	Heap(std::initializer_list<T> init, const Compare& comp = Compare())
		: Heap(init.begin(), init.end(), comp) {}  // 委托给区间构造函数

	/*
	* 拷贝构造函数
	* @param other 源对象
	*/
	Heap(const Heap& other)
		: _ptr(new T[other._capacity]), _length(other._length),
		_capacity(other._capacity), _compare(other._compare) {
		std::copy(other._ptr, other._ptr + _length, _ptr);
	}

	/*
	* 移动构造函数
	* @param other 源对象
	*/
	Heap(Heap&& other) noexcept
		: Heap() {  // 委托默认构造
		swap(*this, other);
	}

	/*
	* 统一处理拷贝和移动赋值
	* @param other 源对象
	*/
	Heap& operator=(Heap other) {  // 根据实参类型自动选择拷贝/移动构造
		swap(*this, other);
		return *this;
	}

	/*
    * 支持聚合初始化赋值
    * @param init 初始化列表
    */
	Heap& operator=(std::initializer_list<T> init) {
		Heap temp(init);
		swap(*this, temp);
		return *this;
	}
    /*
    * 实现交换对象
    * @param first 源对象
    * @param second 目标对象
    */
	friend void swap(Heap& first, Heap& second) noexcept {
		using std::swap;
		swap(first._ptr, second._ptr);
		swap(first._length, second._length);
		swap(first._capacity, second._capacity);
		swap(first._compare, second._compare);
	}

	/*
    * 析构函数
    */ 
	~Heap() { delete[] _ptr; }

	/*
    * @return 是否为空
    */ 
	bool empty() const noexcept { return _length == 0; }

	/*
	* @return 堆大小
	*/
	size_type size() const noexcept { return _length; }

    /*
    * @return 堆数据指针
    */
    T* data() { return _ptr; }

    /*
    * @return 堆内存大小
    */
    size_type capacity() const noexcept { return _capacity; }

    /*
    * @return 堆是否满容量
    */
    bool full() const noexcept { return _length == _capacity; }

	/*
    * 返回首位元素
    * @return 首位元素
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
    * 添加元素
    * @param value 添加的元素
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
    * 批量插入
    * @param first 批量插入的起始迭代器
    * @param last 批量插入的结束迭代器
    */ 
	template<typename InputIt>
	void push_range(InputIt first, InputIt last) {
		size_type count = std::distance(first, last);
		size_type start_index = _length;
		if (_length + count > _capacity) {
			_reserve(_length + count);
		}
		//插入所有元素
		for (; first != last; ++first) {
			_ptr[_length++] = *first;
		}

		//智能选择建堆策略
		if (count == 0) {
			return;
		} else if (count < _length / 4) {
			//插入元素很少 逐个_heapify_up更高效
			for (; start_index < _length; ++start_index) {
				_heapify_up(start_index);
			}
		} else {
			//插入元素较多 整体建堆更高效
			_rebuild_heap();
		}
	}

	/*
    * 破坏性, 删除堆顶元素
    */ 
	void pop() {
		if (empty()) throw std::out_of_range("Heap is empty");
		std::swap(_ptr[0], _ptr[_length - 1]);
		--_length;
		_heapify_down(0);
		_ensure_length();
	}

	/*
	* @brief 破坏性, 会清空部分堆数据
	* @brief 将堆顶元素弹出到输出迭代器中
	* @param first 起始输出迭代器
	* @param last 结束输出迭代器
	*/
	template<typename OutputIt>
	void pop_range(OutputIt first, OutputIt last) {
		while (first != last && !empty()) {
			*first++ = std::move(top());
			pop();
		}
	}

	/*
	* @brief 破坏性, 会清空全部堆数据
	* @brief 将堆排序到输出迭代器中 
	* @param dest 起始输出迭代器
	*/
	template<typename OutputIt>
	void sort_to(OutputIt dest) {
		while (!empty()) {
			*dest++ = std::move(top());
			pop();
		}
	}
	/*
    * 清空堆
    */ 
	void clear() {
		_length = 0;
		_ensure_length();
	}

	/*
	* 堆检查
	* @return 堆排序是否完整
	*/
	bool is_valid_heap() const {
		for (size_type i = 0; i < _length; ++i) {
			size_type left = 2 * i + 1;
			size_type right = 2 * i + 2;

			if (left < _length && _compare(_ptr[left], _ptr[i])) {
				return false;  // 子节点小于父节点
			}
			if (right < _length && _compare(_ptr[right], _ptr[i])) {
				return false;
			}
		}
		return true;
	}

	/*
    * 改变比较函数
    * @param newCompare 新的比较函数
    */
	template<typename NewCompare>
	void change_compare(NewCompare newCompare) {
		_compare = std::function<bool(const T&, const T&)>(newCompare);
		_rebuild_heap();
	}

	/*
	* 改变容量
	* @param new_capacity 新的容量
	*/
	void reserve(size_type new_capacity) {
		if (new_capacity > _capacity) {
			_reserve(new_capacity);
		}
	}

	/*
    * 查找元素 O(n)
    * @param value 要查找的元素
    * @return 找到的元素的指针
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
    * 基于元素指针的更新
    * @param pos 要更新的元素指针
    * @param new_value 新的元素
    * @return 元素指针是否越界
    */ 
	bool update_at(T* pos, const T& new_value) {
		if (pos < _ptr || pos >= _ptr + _length) {
			return false;  // 指针越界检查
		}

		size_type index = static_cast<size_type>(pos - _ptr);
		_ptr[index] = new_value;

		// 重新堆化
		_heapify_up(index);
		_heapify_down(index);
		return true;
	}
    /*
    * 基于元素指针的更新
    * @param pos 要更新的元素指针
    * @param new_value 新的元素
    * @return 元素指针是否越界
    */
	bool update_at(const T* pos, const T& new_value) {
		return update_at(const_cast<T*>(pos), new_value);
	}

	/*
    * 缩减内存到合适大小
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
    * 合并两个堆
    * @param other 另一个堆 (移动堆)
    */
	void merge(Heap&& other) {
		push_range(other.begin(), other.end());
		other.clear();
	}

	//迭代器相关
	T* begin() noexcept { return _ptr; }
	const T* begin() const noexcept { return _ptr; }
	T* end() noexcept { return _ptr + _length; }
	const T* end() const noexcept { return _ptr + _length; }
	// const版本
	const T* cbegin() const noexcept { return _ptr; }
	const T* cend() const noexcept { return _ptr + _length; }
	// 添加反向迭代器支持
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
	* @brief 获取 应该分配的内存大小
	* @param num 内存大小
	* @return 大于num最小的数 max(2^n-1,15)
	*/
	static size_type get_alloc_size(size_type num) {
		if (num == 0) return 0;
		if (num <= 15) return 15;
		return supremum_mask(num);
	}

	/*
	* @param num 输入数
	* @return 大于 num 的2^n-1 的下界
	*/
	static size_type supremum_mask(size_type num) {
		num |= num >> 1;
		num |= num >> 2;
		num |= num >> 4;
		num |= num >> 8;
		num |= num >> 16;
		// 如果是64位类型，需要额外的移位
		if constexpr (sizeof(size_type) > 4) {
			num |= num >> 32;
		}
		return num;
	}
private:
	/*
    * 上浮调整堆
    * @param index 待调整的节点索引
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
    * 下沉调整堆
    * @param index 待调整的节点索引
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
    * @brief 扩展内存
    * @brief 内存为max(2^n-1,15)
    * @param minCapacity 最小容量
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
    * 缩减内存, 长度*4+3 > 内存容量 时触发
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
    * Floyd 建堆 重新构建堆
    */  
	void _rebuild_heap() {
		if (_length <= 1) return;
		size_type i = (_length - 1) / 2;
		while (true) { 
			// 循环从最后一个非叶子节点开始，自底向上进行堆化
			_heapify_down(i);
			if (i == 0) break;
			--i;
		}
	}
};

void test_heap_operations() {
	std::cout << "=== 测试基本堆操作 ===" << std::endl;

   //测试默认构造
	Heap<int> heap1;
	assert(heap1.empty());
	assert(heap1.size() == 0);

   //测试插入和top
	heap1.push(5);
	heap1.push(2);
	heap1.push(8);
	heap1.push(1);

	assert(heap1.size() == 4);
	assert(heap1.top() == 1);  // 最小堆，应该是最小值

   //测试pop
	heap1.pop();
	assert(heap1.top() == 2);
	heap1.pop();
	assert(heap1.top() == 5);

	std::cout << "基本堆操作测试通过" << std::endl;
}

void test_initialization() {
	std::cout << "\n=== 测试初始化 ===" << std::endl;

   //测试初始化列表
	Heap<int> heap2{ 3, 1, 4, 1, 5, 9, 2 };
	assert(heap2.top() == 1);

   //测试迭代器构造
	std::vector<int> vec{ 6, 3, 8, 2, 9 };
	Heap<int> heap3(vec.begin(), vec.end());
	assert(heap3.top() == 2);

   //测试拷贝构造
	Heap<int> heap4 = heap3;
	assert(heap4.top() == 2);
	heap3.pop();
	assert(heap4.top() == 2);  // 拷贝应该独立

   //测试移动构造
	Heap<int> heap5 = std::move(heap4);
	assert(heap5.top() == 2);
	assert(heap4.empty());  // 移动后原对象应为空

	std::cout << "初始化测试通过" << std::endl;
}

void test_capacity_management() {
	std::cout << "\n=== 测试容量管理 ===" << std::endl;

	Heap<int> heap;
	size_t initial_capacity = heap.capacity();
	std::cout << "初始容量: " << initial_capacity << std::endl;

   //测试扩容
	for (int i = 0; i < 20; ++i) {
		heap.push(i);
	}
	std::cout << "插入20个元素后容量: " << heap.capacity() << std::endl;
	assert(heap.capacity() > initial_capacity);

   //测试缩容
	for (int i = 0; i < 15; ++i) {
		heap.pop();
	}
	std::cout << "删除15个元素后容量: " << heap.capacity() << std::endl;

	std::cout << "容量管理测试通过" << std::endl;
}

void test_custom_comparator() {
	std::cout << "\n=== 测试自定义比较器 ===" << std::endl;

	//最大堆
	Heap<int, std::greater<int>> max_heap;
	max_heap.push(1);
	max_heap.push(3);
	max_heap.push(2);

	assert(max_heap.top() == 3);  // 最大堆，应该是最大值

   //测试更改比较器
	Heap<int> min_heap{ 5, 2, 8 };
	assert(min_heap.top() == 2);

	min_heap.change_compare(std::greater<int>());
	assert(min_heap.top() == 8);  // 现在应该是最大值

	std::cout << "自定义比较器测试通过" << std::endl;
}

void test_edge_cases() {
	std::cout << "\n=== 测试边界情况 ===" << std::endl;

   //测试空堆
	Heap<int> empty_heap;
	try {
		empty_heap.top();
		assert(false);  // 应该抛出异常
	} catch (const std::out_of_range&) {
		std::cout << "空堆top()异常测试通过" << std::endl;
	}

	try {
		empty_heap.pop();
		assert(false);  // 应该抛出异常
	} catch (const std::out_of_range&) {
		std::cout << "空堆pop()异常测试通过" << std::endl;
	}

   //测试单个元素
	Heap<int> single_heap;
	single_heap.push(42);
	assert(single_heap.top() == 42);
	single_heap.pop();
	assert(single_heap.empty());

   //测试重复元素
	Heap<int> dup_heap{ 2, 2, 2, 1, 1, 3 };
	assert(dup_heap.top() == 1);
	dup_heap.pop();
	assert(dup_heap.top() == 1);

	std::cout << "边界情况测试通过" << std::endl;
}

void test_performance() {
	std::cout << "\n=== 测试性能 ===" << std::endl;

	const int N = 1000;
	Heap<int> heap;

	//批量插入测试
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
	std::cout << "插入 " << N << " 个元素耗时: " << duration.count() << " μs" << std::endl;

	//验证堆属性
	int last = heap.top();
	while (!heap.empty()) {
		int current = heap.top();
		heap.pop();
		assert(current >= last);  // 对于最小堆，弹出的元素应该非递减
		last = current;
	}

	std::cout << "性能测试通过" << std::endl;
}

void test_iterators() {
	std::cout << "\n=== 测试迭代器 ===" << std::endl;

	Heap<int> heap{ 3, 1, 4, 1, 5 };

   //测试范围for循环
	std::cout << "堆中元素: ";
	for (const auto& elem : heap) {
		std::cout << elem << " ";
	}
	std::cout << std::endl;

   //测试标准算法
	auto it = std::find(heap.begin(), heap.end(), 4);
	assert(it != heap.end());
	assert(*it == 4);

	std::cout << "迭代器测试通过" << std::endl;
}
