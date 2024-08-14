#pragma once
#include "myHead.h"

template<typename T>//���Զ���
class StaticQueue {
    /*
    capacity=5
    0a 1b 2c 3d 4  
    0c 1d 2  3a 4b 
    �ճ�һλ�����ڴ�������
    ������ʱ
    rear - front = capacity - 1= -1(mod capacity)
    */
protected:
    size_t front;//����
    size_t rear;//��β
    size_t capacity;//�����ڴ泤�� 
    T* list;
public:
    //���캯��
    StaticQueue(size_t initialSize = 0) 
        :capacity(initialSize), list(capacity ? new T[capacity] : nullptr){}
    //����size��inԪ��
    StaticQueue(size_t initialSize, const T& in)
        :capacity(initialSize ? initialSize + 1 : 0), list(capacity ? new T[capacity] : nullptr) {
        for (rear = 0; rear < initialSize; ++rear) {
            list[rear] = in;
        }
    }
    //�б��ʼ��
    StaticQueue(std::initializer_list<T> in) 
        :capacity(in.size() ? in.size() + 1 : 0), list(capacity ? new T[capacity] : nullptr) {
        for (const T* it = in.begin(); it != in.end(); ++it) {
            list[rear] = *it;
            ++rear;
        }
    }
    //��������
    StaticQueue(const StaticQueue& other)
        :capacity(other.capacity), list(capacity ? new T[capacity] : nullptr), front(other.front), rear(other.front) {
        for (rear = other.front; rear != other.rear; rear %= other.capacity) {
            list[rear] = other.list[rear];
            ++rear;
        }
    }
    //�ƶ�����
    StaticQueue(StaticQueue&& other) noexcept
        : list(other.list), front(other.front), rear(other.rear), capacity(other.capacity) {
        other.list = nullptr;
        other.front = 0;
        other.rear = 0;
        other.capacity = 0;
    }
    //��������
    ~StaticQueue() {
        delete[] list;
    }
    //������ֵ
    StaticQueue<T>& operator=(const StaticQueue& other) {
        if (this != &other) {
            delete[] list;
            capacity = other.capacity;
            if (capacity) {
                list = new T[capacity];
                front = other.front;
                for (rear = other.front; rear != other.rear; rear %= other.capacity) {
                    list[rear] = other.list[rear];
                    ++rear;
                }
            }
            else {
                list = nullptr;
            }
        }
        return *this;
    }
    //�ƶ���ֵ
    StaticQueue<T>& operator=(StaticQueue&& other) noexcept {
        if (this != &other) {
            delete[] list;
            list = other.list;
            front = other.front;
            rear = other.rear;
            capacity = other.capacity;
            other.list = nullptr;
            other.front = 0;
            other.rear = 0;
            other.capacity = 0;
        }
        return *this;
    }
    //���ض��г���
    inline size_t size() const {
        return (rear + capacity - front) % capacity;
    }
    //�������Ϊ�� ����TRUE
    inline bool empty() const {
        return rear == front;
    }
    //����Ƿ�װ��
    inline bool _isFull() const {//rear - front = capacity - 1 (mod capacity)
        return ((front + capacity - rear) % capacity) == 1;//front - rear = 1
    }
    //�����ڴ泤�� 
    inline size_t _getCapacity() const {
        return capacity;
    }
    //��ն���
    inline void clear() {
        rear = front;
    }
    //���ض�βԪ��
    inline T& back() {
        if (empty()) {
#ifdef DEBUG
                throw std::out_of_range("Queue is empty");
#endif // DEBUG
            return nullRef<T>();
        }
        size_t temp = (rear + capacity - 1) % capacity;
        return list[temp];
    }
    inline const T& back() const{
        if (empty()) {
#ifdef DEBUG
            throw std::out_of_range("Queue is empty");
#endif // DEBUG
            return nullRef<T>();
        }
        size_t temp = (rear + capacity - 1) % capacity;
        return list[temp];
    }
    //��������
    //�ڴ��ط���
    void _resizeCapacity(size_t newCapacity) {
        if (newCapacity <= size() + 1) {
#ifdef DEBUG
            throw std::out_of_range("Capacity less than length");
#endif //DEBUG
            return;
        }
        T* newList = new T[newCapacity];
        size_t newRear = 0;
        for (size_t temp = front; temp != rear; temp %= capacity) {
            newList[newRear] = std::move(list[temp]);
            ++newRear;
            ++temp;
        }
        delete[] list;
        list = newList;
        front = 0;
        rear = newRear;
        capacity = newCapacity;
        return;
    }
    //���
    void push(const T& value) {
        if (_isFull()) {
            capacity *= 2;
            _resizeCapacity(capacity);
        }
        list[rear] = value;
        rear = (rear + 1) % capacity;
        return;
    }
    //����
    void pop() {
        if (empty()) {
            return;
        }
        if (size() < capacity / 4 && capacity >= 32) {
            capacity /= 2;
            _resizeCapacity(capacity);
        }
        front = (front + 1) % capacity;
        return;
    }
    //������Ԫ��ִ��func
    template<typename Function>
    inline void forEachFunction(Function func) {
        for (T* temp = front; temp != rear; temp %= capacity) {
            func(list[temp]);
            ++temp;
        }
    }
};
