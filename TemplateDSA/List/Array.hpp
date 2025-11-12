#pragma once
#include <cstddef>          //size_t
#include <iterator>         //std::reverse_iterator
#include <stdexcept>        //std::out_of_range
#include <initializer_list> //std::initializer_list<>
#include "../allocator.hpp"


template <class T, size_t N>
class Array {
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using iterator = T*;
	using const_iterator = const T*;
	using reverse_iterator = std::reverse_iterator<T*>;
	using const_reverse_iterator = std::reverse_iterator<const T*>;
	T _elements[N]; //固定长度数组
public:
	/*
	* @brief 默认构造函数
	*/
	Array() = default;
	/*
	* @brief 数组构造函数
	* @param init 初始化原始数组
	*/
	constexpr Array(const T(&init)[N]) {
		for (size_t i = 0; i < N; ++i) {
			_elements[i] = init[i];
		}
	}
    /*
    * 显示移除数组长度不匹配的 数组构造函数
    */
    template <size_t M>
    constexpr Array(const T(&)[M]) = delete;
    /*
    * @brief 初始化列表构造
    * @param init 初始化列表
    */ 
    Array(std::initializer_list<T> init) {
        if (init.size() != N) {
            throw std::invalid_argument("Initializer list size mismatch");
        }
        size_t i = 0;
        for (const auto& item : init) {
            _elements[i++] = item;
        }
    }
    /*
    * @brief 拷贝构造函数
    * @param init 初始化原始数组
    */
	Array(const Array& other) {
		for (size_t i = 0; i < N; ++i) {
			_elements[i] = other._elements[i];
		}
	}
    /*
    * @brief 移动构造函数
    * @param init 初始化原始数组
    */
    Array(Array&& other)  noexcept {
        for (size_t i = 0; i < N; ++i) {
            _elements[i] = std::move(other._elements[i]);
        }
    }
    /*
    * 显示移除长度不匹配的 拷贝/移动构造函数
    */
    template <size_t M>
    Array(const Array<T, M>&) = delete;
    template <size_t M>
    Array(Array<T, M>&&) = delete;
	/*
	* 统一处理拷贝和移动赋值
	* @param other 源对象（根据参数类型自动选择拷贝/移动构造）
	*/
	Array& operator=(Array other) {
		swap(*this, other);
		return *this;
	}
	/*
	* 友元swap函数，直接交换所有成员
	*/
	friend void swap(Array& lhs, Array& rhs) noexcept {
		using std::swap;
		for (size_t i = 0; i < N; ++i) {
			swap(lhs._elements[i], rhs._elements[i]);
		}
	}

//主结构操作函数
public:

	/*
	* 索引访问
	* @param index 索引
	* @return 索引位置元素引用
	*/
	T& operator[](size_t index) noexcept {
		return _elements[index];
	}
	const T& operator[](size_t index) const noexcept {
		return _elements[index];
	}
	/*
	* 安全索引访问
	* @param index 索引
	* @return 索引位置元素引用
	* @throw std::out_of_range 索引越界
	*/
	T& at(size_t index) {
		if (index >= N)
			throw std::out_of_range("Index out of range Array::at");
		return _elements[index];
	}
	const T& at(size_t index) const {
		if (index >= N)
			throw std::out_of_range("Index out of range Array::at");
		return _elements[index];
	}
	/*
	* 填充全部数组
	* @param value 填充值
	*/
	void fill(const T& value) noexcept {
		for (size_t i = 0; i < N; ++i) {
			_elements[i] = value;
		}
	}
	/*
	* 访问首位元素
	* @return 首位元素引用
	*/
	T& front() noexcept {
		return _elements[0];
	}
	const T& front() const noexcept {
		return _elements[0];
	}
	/*
	* 访问末尾元素
	* @return 末尾元素引用
	*/
	T& back() noexcept {
		return _elements[N - 1];
	}
	const T& back() const noexcept {
		return _elements[N - 1];
	}

//内存操作函数
public:
	/*
	* @return 数组长度是否为0
	*/
	static constexpr bool empty() noexcept {
		return false;
	}
	/*
	* @return 数组长度
	*/
	static constexpr size_t size() noexcept {
		return N;
	}
	/*
	* @return 最大数组长度
	*/
	static constexpr size_t max_size() noexcept {
		return N;
	}
	/*
	* @return 返回指向数组的指针
	*/
	T* data() noexcept {
		return _elements;
	}
	const T* data() const noexcept {
		return _elements;
	}
	const T* cdata() const noexcept {
		return _elements;
	}

//迭代器相关
public:

	/*
	* @return 返回指向首元素的迭代器
	*/
	T* begin() noexcept {
		return _elements;
	}
	const T* begin() const noexcept {
		return _elements;
	}
	/*
	* @return 返回指向尾元素的迭代器
	*/
	T* end() noexcept {
		return _elements + N;
	}
	const T* end() const noexcept {
		return _elements + N;
	}
	/*
	* @return 反向迭代器首位
	*/
	std::reverse_iterator<T*> rbegin() noexcept {
		return std::make_reverse_iterator(_elements + N);
	}
	std::reverse_iterator<const T*> rbegin() const noexcept {
		return std::make_reverse_iterator(_elements + N);
	}
	/*
	* @return 反向迭代器尾位
	*/
	std::reverse_iterator<T*> rend() noexcept {
		return std::make_reverse_iterator(_elements);
	}
	std::reverse_iterator<const T*> rend() const noexcept {
		return std::make_reverse_iterator(_elements);
	}

	const T* cbegin() const noexcept {
		return _elements;
	}
	const T* cend() const noexcept {
		return _elements + N;
	}
	std::reverse_iterator<const T*> crbegin() const noexcept {
		return std::make_reverse_iterator(_elements + N);
	}
	std::reverse_iterator<const T*> crend() const noexcept {
		return std::make_reverse_iterator(_elements);
	}
};
//空数组模板特化
template <class T>
class Array<T, 0> {
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using iterator = T*;
	using const_iterator = const T*;
	using reverse_iterator = T*;
	using const_reverse_iterator = const T*;
    Array& operator=(Array other) noexcept {
        return *this;
    }
	T& operator[](size_t index) noexcept {
		return *nullptr;
	}
	const T& operator[](size_t index) const noexcept {
		return *nullptr;
	}
	T& at(size_t index) {
		throw std::out_of_range("Index out of range Array<0>::at");
	}
	T const& at(size_t index) const {
		throw std::out_of_range("Index out of range Array<0>::at");
	}
	void fill(const T&) noexcept {}
    friend void swap(Array& lhs, Array& rhs) noexcept {}
	T& front() noexcept {
		return *nullptr;
	}
	const T& front() const noexcept {
		return *nullptr;
	}
	T& back() noexcept {
		return *nullptr;
	}
	const T& back() const noexcept {
		return *nullptr;
	}
	static constexpr bool empty() noexcept {
		return true;
	}
	static constexpr size_t size() noexcept {
		return 0;
	}
	static constexpr size_t max_size() noexcept {
		return 0;
	}
	T* data() noexcept {
		return nullptr;
	}
	const T* data() const noexcept {
		return nullptr;
	}
	const T* cdata() const noexcept {
		return nullptr;
	}
	T* begin() noexcept {
		return nullptr;
	}
	T* end() noexcept {
		return nullptr;
	}
	const T* begin() const noexcept {
		return nullptr;
	}
	const T* end() const noexcept {
		return nullptr;
	}
	const T* cbegin() const noexcept {
		return nullptr;
	}
	const T* cend() const noexcept {
		return nullptr;
	}
	T* rbegin() noexcept {
		return nullptr;
	}
	T* rend() noexcept {
		return nullptr;
	}
	const T* rbegin() const noexcept {
		return nullptr;
	}
	const T* rend() const noexcept {
		return nullptr;
	}
	const T* crbegin() const noexcept {
		return nullptr;
	}
	const T* crend() const noexcept {
		return nullptr;
	}
};

#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>

void test_array_construction() {
    std::cout << "=== 测试构造 ===" << std::endl;

    // 测试默认构造
    Array<int, 5> arr1;
    assert(arr1.size() == 5);

    // 测试初始化列表构造
    Array<int, 3> arr2{ 1, 2, 3 };
    assert(arr2[0] == 1 && arr2[1] == 2 && arr2[2] == 3);

    // 测试原始数组构造
    int raw_arr[4] = { 10, 20, 30, 40 };
    Array<int, 4> arr3(raw_arr);
    assert(arr3[0] == 10 && arr3[3] == 40);

    std::cout << "构造测试通过" << std::endl;
}

void test_array_copy_move() {
    std::cout << "\n=== 测试拷贝移动 ===" << std::endl;

    Array<int, 3> original{ 1, 2, 3 };

    // 测试拷贝构造
    Array<int, 3> copy = original;
    assert(copy[0] == 1 && copy[2] == 3);

    // 测试移动构造
    Array<int, 3> moved = std::move(original);
    assert(moved[0] == 1 && moved[2] == 3);

    // 测试统一赋值
    Array<int, 3> assigned;
    assigned = copy;
    assert(assigned[1] == 2);

    std::cout << "拷贝移动测试通过" << std::endl;
}

void test_array_access() {
    std::cout << "\n=== 测试访问 ===" << std::endl;

    Array<int, 3> arr{ 10, 20, 30 };

    // 测试operator[]
    assert(arr[0] == 10);
    arr[1] = 99;
    assert(arr[1] == 99);

    // 测试at()安全访问
    assert(arr.at(2) == 30);
    try {
        arr.at(5);
        assert(false);
    } catch (const std::out_of_range&) {
        std::cout << "at()边界检查通过" << std::endl;
    }

    // 测试front/back
    assert(arr.front() == 10);
    assert(arr.back() == 30);

    std::cout << "访问测试通过" << std::endl;
}

void test_array_swap() {
    std::cout << "\n=== 测试swap ===" << std::endl;

    Array<int, 3> a{ 1, 2, 3 };
    Array<int, 3> b{ 4, 5, 6 };

    swap(a, b);
    assert(a[0] == 4 && b[0] == 1);

    swap(a,b);
    assert(a[0] == 1 && b[0] == 4);

    std::cout << "swap测试通过" << std::endl;
}

void test_array_fill() {
    std::cout << "\n=== 测试fill ===" << std::endl;

    Array<int, 5> arr;
    arr.fill(42);

    for (size_t i = 0; i < arr.size(); ++i) {
        assert(arr[i] == 42);
    }

    std::cout << "fill测试通过" << std::endl;
}

void test_array_iterators() {
    std::cout << "\n=== 测试迭代器 ===" << std::endl;

    Array<int, 4> arr{ 1, 2, 3, 4 };

    // 测试正向迭代
    int sum = 0;
    for (const auto& elem : arr) {
        sum += elem;
    }
    assert(sum == 10);

    // 测试反向迭代
    sum = 0;
    for (auto it = arr.rbegin(); it != arr.rend(); ++it) {
        sum += *it;
    }
    assert(sum == 10);

    // 测试STL算法
    auto it = std::find(arr.begin(), arr.end(), 3);
    assert(it != arr.end());
    assert(*it == 3);

    std::cout << "迭代器测试通过" << std::endl;
}

void test_size_safety() {
    std::cout << "\n=== 测试大小安全 ===" << std::endl;

    Array<int, 3> arr3;
    Array<int, 5> arr5;

    // 这些应该编译通过
    Array<int, 3> copy3 = arr3;
    Array<int, 5> copy5 = arr5;

    // 这些应该编译失败（取消注释测试）
    // Array<int, 3> error1 = arr5;  // 不同大小 - 应该编译错误
    // Array<int, 3> error2{1, 2};   // 初始化列表大小不匹配 - 应该运行时异常

    std::cout << "大小安全检查通过" << std::endl;
}

void test_empty_array() {
    std::cout << "\n=== 测试空数组 ===" << std::endl;

    Array<int, 0> empty;
    assert(empty.empty());
    assert(empty.size() == 0);

    try {
        empty.at(0);
        assert(false);
    } catch (const std::out_of_range&) {
        std::cout << "空数组边界检查通过" << std::endl;
    }

    // 测试空数组的swap和赋值
    Array<int, 0> empty2;
    empty = empty2;  // 应该正常工作
    swap(empty, empty2);  // 应该正常工作

    std::cout << "空数组测试通过" << std::endl;
}

void test_complex_types() {
    std::cout << "\n=== 测试复杂类型 ===" << std::endl;

    Array<std::string, 3> str_arr{ "hello", "world", "!" };
    assert(str_arr[0] == "hello");
    assert(str_arr.back() == "!");

    // 测试移动语义
    Array<std::string, 3> moved_arr{ std::move(str_arr) };
    assert(moved_arr[0] == "hello");

    std::cout << "复杂类型测试通过" << std::endl;
}

int test_array() {
    try {
        test_array_construction();
        test_array_copy_move();
        test_array_access();
        test_array_swap();
        test_array_fill();
        test_array_iterators();
        test_size_safety();
        test_empty_array();
        test_complex_types();

        std::cout << "\n所有Array测试通过！" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}