#include <functional>
template<typename T, typename Compare = std::less<T>>
class Heap {
public:
    using size_type = std::size_t;
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
private:
    T* _ptr;          // Ԫ������ָ��
    size_type _length;   // Ԫ������
    size_type _capacity; // �����ڴ����� (����2^n-1�ṹ)
    Compare _compare;     // �ȽϺ�������
    static constexpr size_type MIN_CAPACITY = 15; //��С����

public:
    // ���캯��
    explicit Heap(size_type initialCapacity = 15
        , const Compare& comp = Compare())
        : _ptr(nullptr), _length(0),
        _capacity(initialCapacity > 0 ? nextPowerOfTwoMinusOne(initialCapacity) : 0), _compare(comp) {
        if (_capacity > 0) {
            _ptr = new T[_capacity];
        }
    }

    //��������Χ����
    template<typename InputIt>
    Heap(InputIt first, InputIt last, const Compare& comp = Compare())
        : Heap(std::distance(first, last), comp) {
        push_range(first, last);
    }

    // ��ʼ���б���
    Heap(std::initializer_list<T> init, const Compare& comp = Compare())
        : Heap(init.begin(), init.end(), comp) {}  // ί�и����乹�캯��

    // �������캯��
    Heap(const Heap& other)
        : _ptr(new T[other._capacity]), _length(other._length),
        _capacity(other._capacity), _compare(other._compare) {
        std::copy(other._ptr, other._ptr + _length, _ptr);
    }

    // �ƶ����캯��
    Heap(Heap&& other) noexcept
        : Heap() {  // ί��Ĭ�Ϲ���
        swap(*this, other);
    }

    // ͳһ���������ƶ���ֵ
    Heap& operator=(Heap other) {  // ����ʵ�������Զ�ѡ�񿽱�/�ƶ�����
        swap(*this, other);
        return *this;
    }

    // ֧�־ۺϳ�ʼ����ֵ
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

    // ��������
    ~Heap() { delete[] _ptr; }

    // �Ƿ�Ϊ��
    bool empty() const noexcept { return _length == 0; }

    // ����Ԫ������
    size_type size() const noexcept { return _length; }

    // ������λԪ��
    T& top() {
        if (empty()) throw std::out_of_range("Heap is empty");
        return _ptr[0];
    }

    const T& top() const noexcept {
        if (empty()) throw std::out_of_range("Heap is empty");
        return _ptr[0];
    }

    // ���Ԫ��
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

    // ��������
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

    // ɾ���Ѷ�Ԫ��
    void pop() {
        if (empty()) throw std::out_of_range("Heap is empty");
        std::swap(_ptr[0], _ptr[_length - 1]);
        --_length;
        _heapify_down(0);
        _ensure_length();
    }

    // ��ն�
    void clear() {
        _length = 0;
        _ensure_length();
    }

    //��������ָ��
    T* data() { return _ptr; }

    //�������
    size_type capacity() const noexcept { return _capacity; }
    bool full() const noexcept { return _length == _capacity; }

    // ���ıȽϺ��������¹�����
    template<typename NewCompare>
    void changeCompare(NewCompare newCompare) {
        _compare = newCompare;
        _rebuildHeap();
    }

    //���������
    T* begin() noexcept { return _ptr; }
    const T* begin() const noexcept { return _ptr; }
    T* end() noexcept { return _ptr + _length; }
    const T* end() const noexcept { return _ptr + _length; }

    //��ȡ ���� num ��2^n-1 ���½� (���Ի���2�ַ�)
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
    // �������ϵ�����
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

    // �������µ�����
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

    // ��չ�ڴ�
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

    // �����ڴ�
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

    // ���¹�����
    void _rebuildHeap() {
        if (_length <= 1) return;
        // ���ڿ��԰�ȫ��ʹ���з���ѭ��
        for (size_type i = (_length - 1) / 2; i >= 0; --i) {
            _heapify_down(i);
            if (i == 0) break;
        }
    }
};