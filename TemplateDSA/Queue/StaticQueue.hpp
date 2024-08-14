#pragma once
#include "myHead.h"

template<typename T>//线性队列
class StaticQueue {
    /*
    capacity=5
    0a 1b 2c 3d 4  
    0c 1d 2  3a 4b 
    空出一位用作内存存量检测
    满队列时
    rear - front = capacity - 1= -1(mod capacity)
    */
protected:
    size_t front;//队首
    size_t rear;//队尾
    size_t capacity;//分配内存长度 
    T* list;
public:
    //构造函数
    StaticQueue(size_t initialSize = 0) 
        :capacity(initialSize), list(capacity ? new T[capacity] : nullptr){}
    //创建size个in元素
    StaticQueue(size_t initialSize, const T& in)
        :capacity(initialSize ? initialSize + 1 : 0), list(capacity ? new T[capacity] : nullptr) {
        for (rear = 0; rear < initialSize; ++rear) {
            list[rear] = in;
        }
    }
    //列表初始化
    StaticQueue(std::initializer_list<T> in) 
        :capacity(in.size() ? in.size() + 1 : 0), list(capacity ? new T[capacity] : nullptr) {
        for (const T* it = in.begin(); it != in.end(); ++it) {
            list[rear] = *it;
            ++rear;
        }
    }
    //拷贝构造
    StaticQueue(const StaticQueue& other)
        :capacity(other.capacity), list(capacity ? new T[capacity] : nullptr), front(other.front), rear(other.front) {
        for (rear = other.front; rear != other.rear; rear %= other.capacity) {
            list[rear] = other.list[rear];
            ++rear;
        }
    }
    //移动构造
    StaticQueue(StaticQueue&& other) noexcept
        : list(other.list), front(other.front), rear(other.rear), capacity(other.capacity) {
        other.list = nullptr;
        other.front = 0;
        other.rear = 0;
        other.capacity = 0;
    }
    //析构函数
    ~StaticQueue() {
        delete[] list;
    }
    //拷贝赋值
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
    //移动赋值
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
    //返回队列长度
    inline size_t size() const {
        return (rear + capacity - front) % capacity;
    }
    //如果队列为空 返回TRUE
    inline bool empty() const {
        return rear == front;
    }
    //检测是否装满
    inline bool _isFull() const {//rear - front = capacity - 1 (mod capacity)
        return ((front + capacity - rear) % capacity) == 1;//front - rear = 1
    }
    //返回内存长度 
    inline size_t _getCapacity() const {
        return capacity;
    }
    //清空队列
    inline void clear() {
        rear = front;
    }
    //返回队尾元素
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
    //队列重整
    //内存重分配
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
    //入队
    void push(const T& value) {
        if (_isFull()) {
            capacity *= 2;
            _resizeCapacity(capacity);
        }
        list[rear] = value;
        rear = (rear + 1) % capacity;
        return;
    }
    //出队
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
    //对所有元素执行func
    template<typename Function>
    inline void forEachFunction(Function func) {
        for (T* temp = front; temp != rear; temp %= capacity) {
            func(list[temp]);
            ++temp;
        }
    }
};
