#pragma once
#include "myHead.h"

template<typename T>//����ջ
class StaticStack {
protected:
    size_t capacity;//�����ڴ泤�� 
    T* stackBase;
    T* stackTop;
    size_t increment = 10;//�ڴ�����
public:
    //���캯��
    StaticStack(size_t initialSize = 0) 
        :capacity(initialSize), stackBase(capacity ? new T[capacity] : nullptr), stackTop(stackBase) {}
    //����size��inԪ��
    StaticStack(size_t initialSize, const T& in) 
        :capacity(initialSize), stackBase(capacity ? new T[capacity] : nullptr), stackTop(stackBase) {
        for (size_t i = 0; i < capacity; ++i) {
            *stackTop = in;
            ++stackTop;
        }
    }
    //�б��ʼ��
    StaticStack(const std::initializer_list<T>& in) 
        :capacity(in.size()), stackBase(capacity ? new T[capacity] : nullptr), stackTop(stackBase) {
        for (const T* it = in.begin(); it != in.end(); ++it) {
            *stackTop = *it;
            ++stackTop;
        }
    }
    //�б��ʼ��
    StaticStack(std::initializer_list<T>&& in) 
        :capacity(in.size()), stackBase(capacity ? new T[capacity] : nullptr), stackTop(stackBase) {
        for (const T* it = in.begin(); it != in.end(); ++it) {
            *stackTop = std::move(*it);
            ++stackTop;
        }
    }
    //��������
    StaticStack(const StaticStack& other) 
        :capacity(other.capacity), stackBase(capacity ? new T[capacity] : nullptr), stackTop(stackBase) {
        for (T* temp = other.stackBase; temp != other.stackTop; ++temp) {
            *stackTop = *temp;
            ++stackTop;
        }
    }
    //�ƶ�����
    StaticStack(StaticStack&& other) noexcept 
        : stackBase(other.stackBase), stackTop(other.stackTop), increment(other.increment), capacity(other.capacity) {
        other.stackBase = nullptr;
        other.stackTop = nullptr;
        other.capacity = 0;
    }
    //��������
    ~StaticStack() {
        delete[] stackBase;
    }
    //������ֵ
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
    //�ƶ���ֵ
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
    //����ջ����
    size_t size() const {
        return stackTop - stackBase;
    }
    //������Ա�Ϊ�� ����TRUE
    bool empty() const {
        return stackTop <= stackBase;
    }
    //�����ڴ泤�� 
    size_t _getCapacity() const {
        return capacity;
    }
    //���ջ
    void clear() {
        stackTop = stackBase;
    }
    //����ջ��Ԫ��
    T& top() {
        if (empty())
            return nullRef<T>();
        return *(stackTop - 1);
    }
    //����ջ��Ԫ��
    const T& top() const{
        if (empty())
            return T();
        return *(stackTop - 1);
    }
    //�ڴ��ط���
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
    //��ջ
    void push(const T& value) {
        if (stackTop - stackBase >= capacity) {
            _resizeCapacity(stackTop - stackBase + increment);
        }
        *stackTop = value;
        ++stackTop;
        return;
    }
    //��ջ
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
    //������Ԫ��ִ��func
    template<typename Function>
    void forEachFunction(Function func) {
        for (T* temp = stackTop - 1; temp >= stackBase; --temp) {
            func(*temp);
        }
    }
};
