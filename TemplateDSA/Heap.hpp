#include <functional>
template<typename T, typename Compare = std::less<T>>
class Heap {
public:
    using size_type = std::size_t;
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
private:
    T* _ptr;          // 元素数组指针
    size_type _length;   // 元素数量
    size_type _capacity; // 分配内存容量 (保持2^n-1结构)
    Compare _compare;     // 比较函数对象
    static constexpr size_type MIN_CAPACITY = 15; //最小容量

public:
    // 构造函数
    explicit Heap(size_type initialCapacity = 15
        , const Compare& comp = Compare())
        : _ptr(nullptr), _length(0),
        _capacity(initialCapacity > 0 ? nextPowerOfTwoMinusOne(initialCapacity) : 0), _compare(comp) {
        if (_capacity > 0) {
            _ptr = new T[_capacity];
        }
    }

    //迭代器范围构造
    template<typename InputIt>
    Heap(InputIt first, InputIt last, const Compare& comp = Compare())
        : Heap(std::distance(first, last), comp) {
        push_range(first, last);
    }

    // 初始化列表构造
    Heap(std::initializer_list<T> init, const Compare& comp = Compare())
        : Heap(init.begin(), init.end(), comp) {}  // 委托给区间构造函数

    // 拷贝构造函数
    Heap(const Heap& other)
        : _ptr(new T[other._capacity]), _length(other._length),
        _capacity(other._capacity), _compare(other._compare) {
        std::copy(other._ptr, other._ptr + _length, _ptr);
    }

    // 移动构造函数
    Heap(Heap&& other) noexcept
        : Heap() {  // 委托默认构造
        swap(*this, other);
    }

    // 统一处理拷贝和移动赋值
    Heap& operator=(Heap other) {  // 根据实参类型自动选择拷贝/移动构造
        swap(*this, other);
        return *this;
    }

    // 支持聚合初始化赋值
    Heap& operator=(std::initializer_list<T> init) {
        Heap temp(init);
        swap(*this, temp);
        return *this;
    }

    friend void swap(Heap& first, Heap& second) noexcept {
        using std::swap;
        swap(first._ptr, second._ptr);
        swap(first._length, second._length);
        swap(first._capacity, second._capacity);
        swap(first._compare, second._compare);
    }

    // 析构函数
    ~Heap() { delete[] _ptr; }

    // 是否为空
    bool empty() const noexcept { return _length == 0; }

    // 返回元素数量
    size_type size() const noexcept { return _length; }

    // 返回首位元素
    T& top() {
        if (empty()) throw std::out_of_range("Heap is empty");
        return _ptr[0];
    }

    const T& top() const noexcept {
        if (empty()) throw std::out_of_range("Heap is empty");
        return _ptr[0];
    }

    // 添加元素
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

    // 批量插入
    template<typename InputIt>
    void push_range(InputIt first, InputIt last) {
        size_type count = std::distance(first, last);
        if (_length + count > _capacity) {
            _reserve(_length + count);
        }
        for (; first != last; ++first) {
            _ptr[_length++] = *first;
            _heapify_up(_length - 1);
        }
    }

    // 删除堆顶元素
    void pop() {
        if (empty()) throw std::out_of_range("Heap is empty");
        std::swap(_ptr[0], _ptr[_length - 1]);
        --_length;
        _heapify_down(0);
        _ensure_length();
    }

    // 清空堆
    void clear() {
        _length = 0;
        _ensure_length();
    }

    //返回数据指针
    T* data() { return _ptr; }

    //容量检查
    size_type capacity() const noexcept { return _capacity; }
    bool full() const noexcept { return _length == _capacity; }

    // 更改比较函数并重新构建堆
    template<typename NewCompare>
    void changeCompare(NewCompare newCompare) {
        _compare = newCompare;
        _rebuildHeap();
    }

    //迭代器相关
    T* begin() noexcept { return _ptr; }
    const T* begin() const noexcept { return _ptr; }
    T* end() noexcept { return _ptr + _length; }
    const T* end() const noexcept { return _ptr + _length; }

    //获取 大于 num 的2^n-1 的下界 (可以换成2分法)
    static size_type nextPowerOfTwoMinusOne(size_type num) {
        if (num == 0) return 0;
        if (num <= 15) return 15;
        size_type power = 1;
        while (power - 1 < num) {
            power *= 2;
        }
        return power - 1;
    }
private:
    // 从下向上调整堆
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

    // 从上向下调整堆
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

    // 扩展内存
    void _reserve(size_type minCapacity = 0) {
        size_type newCapacity = (_capacity == 0) ? MIN_CAPACITY : _capacity * 2 + 1;
        if (minCapacity > newCapacity) {
            newCapacity = nextPowerOfTwoMinusOne(minCapacity);
        }
        T* newPtr = new T[newCapacity];
        for (size_type i = 0; i < _length; ++i) {
            newPtr[i] = std::move(_ptr[i]);
        }
        delete[] _ptr;
        _ptr = newPtr;
        _capacity = newCapacity;
    }

    // 缩减内存
    void _ensure_length() {
        if (_capacity  < MIN_CAPACITY || ((_length + 1) * 4) > _capacity + 1) return;
        size_type newCapacity = (_capacity + 1) / 4 - 1;
        T* newPtr = new T[newCapacity];
        for (size_type i = 0; i < _length; ++i) {
            newPtr[i] = std::move(_ptr[i]);
        }
        delete[] _ptr;
        _ptr = newPtr;
        _capacity = newCapacity;
    }

    // 重新构建堆
    void _rebuildHeap() {
        if (_length <= 1) return;
        // 现在可以安全地使用有符号循环
        for (size_type i = (_length - 1) / 2; i >= 0; --i) {
            _heapify_down(i);
            if (i == 0) break;
        }
    }
};