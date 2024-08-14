#pragma once
#include "myHead.h"

template<typename T>//线性栈
class StaticStack {
protected:
    size_t capacity;//分配内存长度 
    T* stackBase;
    T* stackTop;
    size_t increment = 10;//内存增量
public:
    //构造函数
    StaticStack(size_t initialSize = 0) 
        :capacity(initialSize), stackBase(capacity ? new T[capacity] : nullptr), stackTop(stackBase) {}
    //创建size个in元素
    StaticStack(size_t initialSize, const T& in) 
        :capacity(initialSize), stackBase(capacity ? new T[capacity] : nullptr), stackTop(stackBase) {
        for (size_t i = 0; i < capacity; ++i) {
            *stackTop = in;
            ++stackTop;
        }
    }
    //列表初始化
    StaticStack(const std::initializer_list<T>& in) 
        :capacity(in.size()), stackBase(capacity ? new T[capacity] : nullptr), stackTop(stackBase) {
        for (const T* it = in.begin(); it != in.end(); ++it) {
            *stackTop = *it;
            ++stackTop;
        }
    }
    //列表初始化
    StaticStack(std::initializer_list<T>&& in) 
        :capacity(in.size()), stackBase(capacity ? new T[capacity] : nullptr), stackTop(stackBase) {
        for (const T* it = in.begin(); it != in.end(); ++it) {
            *stackTop = std::move(*it);
            ++stackTop;
        }
    }
    //拷贝构造
    StaticStack(const StaticStack& other) 
        :capacity(other.capacity), stackBase(capacity ? new T[capacity] : nullptr), stackTop(stackBase) {
        for (T* temp = other.stackBase; temp != other.stackTop; ++temp) {
            *stackTop = *temp;
            ++stackTop;
        }
    }
    //移动构造
    StaticStack(StaticStack&& other) noexcept 
        : stackBase(other.stackBase), stackTop(other.stackTop), increment(other.increment), capacity(other.capacity) {
        other.stackBase = nullptr;
        other.stackTop = nullptr;
        other.capacity = 0;
    }
    //析构函数
    ~StaticStack() {
        delete[] stackBase;
    }
    //拷贝赋值
    StaticStack<T>& operator=(const StaticStack& other) {
        if (this != &other) {
            delete[] stackBase;
            capacity = other.capacity;
            stackBase = new T[capacity];
            stackTop = stackBase;
            for (T* temp = other.stackBase; temp != other.stackTop; ++temp) {
                *stackTop = *temp;
                ++stackTop;
            }
        }
        return *this;
    }
    //移动赋值
    StaticStack<T>& operator=(StaticStack&& other) noexcept {
        if (this != &other) {
            delete[] stackBase;
            stackBase = other.stackBase;
            capacity = other.capacity;
            other.stackBase = nullptr;
            other.capacity = 0;
        }
        return *this;
    }
    //返回栈长度
    size_t size() const {
        return stackTop - stackBase;
    }
    //如果线性表为空 返回TRUE
    bool empty() const {
        return stackTop <= stackBase;
    }
    //返回内存长度 
    size_t _getCapacity() const {
        return capacity;
    }
    //清空栈
    void clear() {
        stackTop = stackBase;
    }
    //返回栈顶元素
    T& top() {
        if (empty())
            return nullRef<T>();
        return *(stackTop - 1);
    }
    //返回栈顶元素
    const T& top() const{
        if (empty())
            return T();
        return *(stackTop - 1);
    }
    //内存重分配
    void _resizeCapacity(size_t newCapacity) {
        if (newCapacity <= stackTop - stackBase) {
#ifdef DEBUG
            throw std::out_of_range("Capacity less than length");
#endif //DEBUG
            return;
        }
        T* newBase = new T[newCapacity];
        T* newTop = newBase;
        for (T* temp = stackBase; temp != stackTop; ++temp) {
            *newTop = std::move(*temp);
            ++newTop;
        }
        delete[] stackBase;
        stackBase = newBase;
        stackTop = newTop;
        capacity = newCapacity;
        return;
    }
    //入栈
    void push(const T& value) {
        if (stackTop - stackBase >= capacity) {
            _resizeCapacity(stackTop - stackBase + increment);
        }
        *stackTop = value;
        ++stackTop;
        return;
    }
    //出栈
    void pop() {
        if (empty()) {
#ifdef DEBUG
            throw std::out_of_range("Stack is empty, cannot pop.");
#endif // DEBUG
            return ;
        }
        if (stackTop - stackBase < capacity / 4 && capacity >= 32) {
            capacity /= 2;
            _resizeCapacity(capacity);
        }
        --stackTop;
        return;
    }
    //对所有元素执行func
    template<typename Function>
    void forEachFunction(Function func) {
        for (T* temp = stackTop - 1; temp >= stackBase; --temp) {
            func(*temp);
        }
    }
};
