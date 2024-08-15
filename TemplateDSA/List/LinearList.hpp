#pragma once
#include <cstddef>           //size_t
#include <iterator>         //std::reverse_iterator
#include <stdexcept>        //std::out_of_range
#include <initializer_list> //std::initializer_list<>
#include "../allocator.hpp"

namespace sxl {
    template<typename Type, class Alloc = sxl::Allocator<Type>>//���Ա�
    class LinearList {
    public:
        using value_type = Type;
        using allocator_type = Alloc;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = Type*;
        using const_pointer = const Type*;
        using reference = Type&;
        using const_reference = const Type&;
        using iterator = Type*;
        using const_iterator = const Type*;
        using reverse_iterator = std::reverse_iterator<Type*>;
        using const_reverse_iterator = std::reverse_iterator<const Type*>;
    protected:
        Type* _ptr;
        size_t _length;//Ԫ�ظ���
        size_t _capacity;//�����ڴ泤�� 
        Alloc _alloc;

    public:
        LinearList() noexcept :_ptr(nullptr), _length(0), _capacity(0) {}
        //���캯��
        explicit LinearList(size_t n, const Alloc& alloc = Alloc()) : _alloc(alloc) {
            _ptr = _alloc.allocate(n);
            _capacity = _length = n;
            for (size_t i = 0; i < n; ++i) {
                _alloc.construct(&_ptr[i]);
            }
        }
        //����size��inԪ��
        LinearList(size_t n, const Type& value, const Alloc& alloc = Alloc())
            :_alloc(alloc) {
            _ptr = _alloc.allocate(n);
            _capacity = _length = n;
            for (size_t i = 0; i < n; ++i) {
                _alloc.construct(&_ptr[i], value);
            }
        }
        //�б��ʼ��
        LinearList(std::initializer_list<Type> ilist, const Alloc& alloc = Alloc())
            :LinearList(ilist.begin(), ilist.end(), alloc) {}
        //������ʵ���������
        template <typename InputIt>
        LinearList(InputIt first, InputIt last, const Alloc& alloc = Alloc()) : _alloc(alloc) {
            size_t n = last - first;
            _ptr = _alloc.allocate(n);
            _capacity = _length = n;
            for (size_t i = 0; i < n; ++i, ++first) {
                _alloc.construct(&_ptr[i], *first);
            }
        }
        //��������
        LinearList(const LinearList& other)
            :_length(other._length), _capacity(other._length), _alloc(other._alloc) {
            if (_length == 0) {
                _ptr = nullptr;
            }
            else {
                _ptr = _alloc.allocate(_length);
                for (size_t i = 0; i <_length; ++i) {
                    _alloc.construct(&_ptr[i], other._ptr[i]);
                }
            }
        }
        //����������������
        LinearList(const LinearList& other, const Alloc& alloc)
            :_length(other._length), _capacity(other._length), _alloc(alloc) {
            if (_length == 0) {
                _ptr = nullptr;
            }
            else {
                _ptr = _alloc.allocate(_length);
                for (size_t i = 0; i < _length; ++i) {
                    _alloc.construct(&_ptr[i], other._ptr[i]);
                }
            }
        }
        //�ƶ�����
        LinearList(LinearList&& other) noexcept
            : _ptr(other._ptr), _length(other._length), _capacity(other._capacity), _alloc(std::move(other._alloc)) {
            other._ptr = nullptr;
            other._length = 0;
            other._capacity = 0;
        }
        //���������ƶ�����
        LinearList(LinearList&& other, const Alloc& alloc) noexcept
            :_length(other._length), _capacity(other._capacity), _ptr(other._ptr), _alloc(alloc) {
            other._ptr = nullptr;
            other._length = 0;
            other._capacity = 0;
        }
        //��������
        ~LinearList() {
            for (size_t i = 0; i < _length; ++i) {
                _alloc.destroy(&_ptr[i]);
            }
            if (_capacity != 0) {
                _alloc.deallocate(_ptr, _capacity);
            }
        }
        //������ֵ
        LinearList<Type>& operator=(const LinearList& other) {
            if (this != &other) {
                clear();
                if (_capacity < other._length) {
                    _alloc.deallocate(_ptr, _capacity);
                    _ptr = _alloc.allocate(other._length);
                }
                for (size_t i = 0; i < other._length; ++i) {
                    _alloc.construct(&_ptr[i], other._ptr[i]);
                }
            }
            return *this;
        }
        //�ƶ���ֵ
        LinearList<Type>& operator=(LinearList&& other) noexcept {
            if (this != &other) {
                for (size_t i = 0; i < _length; ++i) {
                    _alloc.destroy(&_ptr[i]);
                }
                if (_capacity != 0) {
                    _alloc.deallocate(_ptr, _capacity);
                }
                _ptr = other._ptr;
                _length = other._length;
                _capacity = other._capacity;
                other._ptr = nullptr;
                other._length = 0;
                other._capacity = 0;
            }
            return *this;
        }
        //�س�ʼ���б�
        LinearList<Type>& operator=(std::initializer_list<Type> ilist) {
            assign(ilist.begin(), ilist.end());
        }
        //�������Ա���
        inline size_t size() const noexcept {
            return _length;
        }
        //������Ա�Ϊ�� ����TRUE
        inline bool empty() const noexcept {
            return _length == 0;
        }
        //�����ڴ泤�� 
        inline size_t capacity() const noexcept {
            return _capacity;
        }
        //��ձ�
        void clear() {
            for (size_t i = 0; i < _length; ++i) {
                _alloc.destroy(&_ptr[i]);
            }
            _length = 0;
            return;
        }
        //������Ԫ��
        static constexpr size_t max_size()noexcept {
            return (static_cast<size_t>(-1)) / sizeof(Type);
        }
        //�ӳ���ض�����
        void resize(size_t newLen) {
            if (newLen < _length) {
                //�ضϲ�����
                for (size_t i = newLen; i < _length; ++i) {
                    _alloc.destroy(&_ptr[i]);
                }
            }
            else if (newLen > _length) {
                reserve(newLen);
                for (size_t i = _length; i < newLen; ++i) {
                    _alloc.construct(&_ptr[i]);
                }
            }
            _length = newLen;
            return;
        }
        //�ӳ���ֵ��ض�����
        void resize(size_t newLen,const Type& value) {
            if (newLen < _length) {
                //�ضϲ�����
                for (size_t i = newLen; i < _length; ++i) {
                    _alloc.destroy(&_ptr[i]);
                }
            }
            else if (newLen > _length) {
                reserve(newLen);
                for (size_t i = _length; i < newLen; ++i) {
                    _alloc.construct(&_ptr[i],value);
                }
            }
            _length = newLen;
            return;
        }
        //��ɾ������,�޸��ڴ泤��
        void ensure_capacity(size_t newCap) {
            if (_length == _capacity) {
                return;
            }
            if (newCap < _length) {
                newCap = _length;
            }
            if (newCap == 0) {//length==0
                _alloc.deallocate(_ptr, _capacity);
                _ptr = nullptr;
                _capacity = 0;
                return;
            }
            Type* newPtr = _alloc.allocate(newCap);
            for (size_t i = 0; i < _length; ++i) {
                _alloc.construct(&newPtr[i], std::move_if_noexcept(_ptr[i]));
                _alloc.destroy(&_ptr[i]);
            }
            _alloc.deallocate(_ptr, _capacity);
            _ptr = newPtr;
            _capacity = newCap;
            return;
        }
        //�ӳ��ڴ泤��
        void reserve(size_t newCap) {
            if (newCap > _capacity) {
                newCap = newCap > _capacity * 2 ? newCap : _capacity * 2;
                Type* newPtr = _alloc.allocate(newCap);
                for (size_t i = 0; i < _length; ++i) {
                    _alloc.construct(&newPtr[i], std::move_if_noexcept(_ptr[i]));
                    _alloc.destroy(&_ptr[i]);
                }
                _alloc.deallocate(_ptr, _capacity);
                _ptr = newPtr;
                _capacity = newCap;
            }
            return;
        }
        //�����������ڴ�
        inline void shrink_to_fit() {
            ensure_capacity(_length);
            return;
        }
        //���ط�����
        Alloc get_allocator() const noexcept {
            return _alloc;
        }
        //����Ԫ������
        Type& operator[](size_t i) noexcept {
            return _ptr[i];
        }
        const Type& operator[](size_t i) const noexcept {
            return _ptr[i];
        }
        Type& at(size_t i) {
            if (i >= _length) {
                throw std::out_of_range("Index out of range LinearList::at");
            }
            return _ptr[i];
        }
        const Type& at(size_t i) const {
            if (i >= _length) {
                throw std::out_of_range("Index out of range LinearList::at");
            }
            return _ptr[i];
        }
        //����Ԫ��ָ��
        Type* _atPtr(size_t i) {
            if (i >= _length) {
                throw std::out_of_range("Index out of range LinearList::_atPtr");
                return nullptr;
            }
            return _ptr + i;
        }
        const Type* _atPtr(size_t i) const {
            if (i >= _length) {
                throw std::out_of_range("Index out of range LinearList::_atPtr");
                return nullptr;
            }
            return _ptr + i;
        }
        //������λ
        Type& front()noexcept {
            return _ptr[0];
        }
        const Type& front()const noexcept {
            return _ptr[0];
        }
        //����ĩλ
        Type& back()noexcept {
            return _ptr[_length - 1];
        }
        const Type& back()const noexcept {
            return _ptr[_length - 1];
        }
        //��������ָ��
        Type* data() noexcept {
            return _ptr;
        }
        const Type* data()const noexcept {
            return _ptr;
        }
        const Type* cdata()const noexcept {
            return _ptr;
        }
        //������
        Type* begin() noexcept {
            return _ptr;
        }
        Type* end() noexcept {
            return _ptr + _length;
        }
        const Type* begin() const noexcept {
            return _ptr;
        }
        const Type* end() const noexcept {
            return _ptr + _length;
        }
        const Type* cbegin() const noexcept {
            return _ptr;
        }
        const Type* cend() const noexcept {
            return _ptr + _length;
        }
        //���������
        std::reverse_iterator<Type*> rbegin() noexcept {
            return std::make_reverse_iterator(_ptr + _length);
        }
        std::reverse_iterator<Type*> rend() noexcept {
            return std::make_reverse_iterator(_ptr);
        }
        std::reverse_iterator<const Type*> rbegin() const noexcept {
            return std::make_reverse_iterator(_ptr + _length);
        }
        std::reverse_iterator<const Type*> rend() const noexcept {
            return std::make_reverse_iterator(_ptr);
        }
        std::reverse_iterator<const Type*> crbegin() const noexcept {
            return std::make_reverse_iterator(_ptr + _length);
        }
        std::reverse_iterator<const Type*> crend() const noexcept {
            return std::make_reverse_iterator(_ptr);
        }
        //��ָ��λ��ǰ����Ԫ��
        template <class ...Args>
        Type* emplace(const Type* it, Args &&...args) {
            size_t j = it - _ptr;
            reserve(_length + 1);
            for (size_t i = _length; i > j; --i) {
                _alloc.construct(&_ptr[i], std::move(_ptr[i - 1]));
                _alloc.destroy(&_ptr[i - 1]);
            }
            ++_length;
            _alloc.construct(&_ptr[j], std::forward<Args>(args)...);
            return _ptr + j;
        }
        template <class ...Args>
        size_t emplace(const size_t index, Args &&...args) {
            if (index > _length) {
                throw std::out_of_range("Index out of range LinearList::emplace");
                return;
            }
            reserve(_length + 1);
            for (size_t i = _length; i > index; --i) {
                _alloc.construct(&_ptr[i], std::move(_ptr[i - 1]));
                _alloc.destroy(&_ptr[i - 1]);
            }
            ++_length;
            _alloc.construct(&_ptr[index], std::forward<Args>(args)...);
            return index;
        }
        //��ָ��λ�ò���Ԫ��
        Type* insert(const Type* it, const Type& value) {
            size_t j = it - _ptr;
            reserve(_length + 1);
            for (size_t i = _length; i > j; --i) {
                _alloc.construct(&_ptr[i], std::move(_ptr[i - 1]));
                _alloc.destroy(&_ptr[i - 1]);
            }
            ++_length;
            _alloc.construct(&_ptr[j], value);
            return _ptr + j;
        }
        size_t insert(size_t index, const Type& value) {
            if (index > _length) {
                throw std::out_of_range("Index out of range LinearList::insert");
                return;
            }
            reserve(_length + 1);  
            for (size_t i = _length; i > index; --i) {
                _alloc.construct(&_ptr[i], std::move(_ptr[i - 1]));
                _alloc.destroy(&_ptr[i - 1]);
            }
            ++_length;
            _alloc.construct(&_ptr[index], value);
            return index;
        }
        Type* insert(const Type* it, Type&& value) {
            size_t j = it - _ptr;
            reserve(_length + 1);
            for (size_t i = _length; i > j; --i) {
                _alloc.construct(&_ptr[i], std::move(_ptr[i - 1]));
                _alloc.destroy(&_ptr[i - 1]);
            }
            ++_length;
            _alloc.construct(&_ptr[j], std::move(value));
            return _ptr + j;
        }
        size_t insert(size_t index, Type&& value) {
            if (index > _length) {
                throw std::out_of_range("Index out of range LinearList::insert");
                return;
            }
            reserve(_length + 1);
            for (size_t i = _length; i > index; --i) {
                _alloc.construct(&_ptr[i], std::move(_ptr[i - 1]));
                _alloc.destroy(&_ptr[i - 1]);
            }
            ++_length;
            _alloc.construct(&_ptr[index], std::move(value));
            return index;
        }
        Type* insert(const Type* it, size_t n,const Type& value) {
            size_t j = it - _ptr;
            if (n == 0)
                return const_cast<Type*>(it);
            reserve(_length + n);
            for (size_t i = _length; i > j; --i) {
                _alloc.construct(&_ptr[i + n - 1], std::move(_ptr[i - 1]));
                _alloc.destroy(&_ptr[i - 1]);
            }
            _length = _length + n;
            for (size_t i = j; i < j + n; ++i) {
                _alloc.construct(&_ptr[i], value);
            }
            return _ptr + j;
        }
        size_t* insert(size_t index, size_t n, const Type& value) {
            if (n == 0)
                return index;
            reserve(_length + n);
            for (size_t i = _length; i > index; --i) {
                _alloc.construct(&_ptr[i + n - 1], std::move(_ptr[i - 1]));
                _alloc.destroy(&_ptr[i - 1]);
            }
            _length = _length + n;
            for (size_t i = index; i < index + n; ++i) {
                _alloc.construct(&_ptr[i], value);
            }
            return index;
        }
        template <typename InputIt>
        Type* insert(const Type* it, InputIt first, InputIt last) {
            size_t j = it - _ptr;
            size_t n = last - first;
            if (n == 0)
                return const_cast<Type*>(it);
            reserve(_length + n);
            for (size_t i = _length; i > j; --i) {
                _alloc.construct(&_ptr[i + n - 1], std::move(_ptr[i - 1]));
                _alloc.destroy(&_ptr[i - 1]);
            }
            _length = _length + n;
            for (size_t i = j; i < j + n; ++i) {
                _alloc.construct(&_ptr[i], *first);
                ++first;
            }
            return _ptr + j;
        }
        template <typename InputIt>
        size_t insert(size_t index, InputIt first, InputIt last) {
            size_t n = last - first;
            if (n == 0)
                return index;
            reserve(_length + n);
            for (size_t i = _length; i > index; --i) {
                _alloc.construct(&_ptr[i + n - 1], std::move(_ptr[i - 1]));
                _alloc.destroy(&_ptr[i - 1]);
            }
            _length = _length + n;
            for (size_t i = index; i < index + n; ++i) {
                _alloc.construct(&_ptr[i], *first);
                ++first;
            }
            return index;
        }
        Type* insert(const Type* it, std::initializer_list<Type> ilist) {
            return insert(it, ilist.begin(), ilist.end());
        }
        size_t insert(size_t index, std::initializer_list<Type> ilist) {
            return insert(index, ilist.begin(), ilist.end());
        }
        //�Ƴ�ָ��λ�õ�Ԫ��
        Type* erase(const Type* it){
            size_t n = it - _ptr;
            for (size_t i = n; i < _length - 1; ++i) {
                _ptr[i] = std::move(_ptr[i + 1]);
            }
            --_length;
            _alloc.destroy(&_ptr[_length]);
            return const_cast<Type*>(it); //����һ��ָ��ɾ��Ԫ��֮��ĵ�����
        }
        size_t erase(const size_t index) {
            for (size_t i = index; i < _length - 1; ++i) {
                _ptr[i] = std::move(_ptr[i + 1]);
            }
            --_length;
            _alloc.destroy(&_ptr[_length]);
            return const_cast<size_t>(index);
        }
        //�Ƴ�ָ����Χ��Ԫ��
        Type* erase(const Type* first,const Type* last) {
            size_t n = last - first;
            for (size_t i = first - _ptr; i < _length - n; ++i) {
                _ptr[i] = std::move(_ptr[i + n]);
            }
            for (size_t i = _length - n; i < _length; ++i) {
                _alloc.destroy(&_ptr[i]);
            }
            _length = _length - n;
            return const_cast<Type*>(first);
        }
        size_t erase(const size_t first, const size_t last) {
            size_t n = last - first;
            for (size_t i = first; i < _length - n; ++i) {
                _ptr[i] = std::move(_ptr[i + n]);
            }
            for (size_t i = _length - n; i < _length; ++i) {
                _alloc.destroy(&_ptr[i]);
            }
            _length = _length - n;
            return const_cast<size_t>(first);
        }
        //ĩβ����Ԫ��
        void push_back(const Type& value) {
            if (_length >= _capacity) {
                reserve(_length + 1);
            }
            _alloc.construct(&_ptr[_length], value);
            ++_length;
        }
        void push_back(Type&& value) {
            if (_length >= _capacity) {
                reserve(_length + 1);
            }
            _alloc.construct(&_ptr[_length], std::move(value));
            ++_length;
        }
        template<class ...Args>
        void push_back(Args && ...args) {
            if (_length >= _capacity) {
                reserve(_length + 1);
            }
            _alloc.construct(&_ptr[_length], std::forward<Args>(args)...);;
            ++_length;
        }
        //ĩβ�Ƴ�Ԫ��
        void pop_back() {
            if (empty()) {
                throw std::out_of_range("LinearList is emty");
                return;
            }
            --_length;
            _alloc.destroy(&_ptr[_length]);
        }
        //�ع���
        void assign(size_t n, const Type& value){
            clear();
            reserve(n);
            _length = n;
            for (size_t i = 0; i < n; ++i) {
                _alloc.construct(&_ptr[i], value);
            }
        }
        template <typename InputIt>
        void assign(InputIt first, InputIt last){
            clear();
            size_t n = last - first;
            reserve(n);
            _length = n;
            for (size_t i = 0; i < n; ++i, ++first) {
                _alloc.construct(&_ptr[i], *first);
            }
        }
        void assign(std::initializer_list<Type> ilist) {
            assign(ilist.begin(), ilist.end());
        }
        //����
        void swap(LinearList& other) noexcept {
            Type* temp_ptr = _ptr;
            _ptr = other._ptr;
            other._ptr = temp_ptr;

            size_t temp_length = _length;
            _length = other._length;
            other._length = temp_length;

            size_t temp_capacity = _capacity;
            _capacity = other._capacity;
            other._capacity = temp_capacity;

            Alloc temp_alloc = _alloc;
            _alloc = other._alloc;
            other._alloc = temp_alloc;
        }
        bool operator == (const LinearList& other) const noexcept {
            if (_length != other._length) {
                return false;
            }
            for (size_t i = 0; i < _length; ++i) {
                if (!(_ptr[i] == other._ptr[i])) {
                    return false;
                }
            }
            return true;
        }
        inline bool operator != (const LinearList& other) const noexcept {
            return !(*this == other);
        }
        bool operator<(const LinearList& other) const noexcept {
            size_t minLength = std::min(_length, other._length);
            for (size_t i = 0; i < minLength; ++i) {
                if (_ptr[i] < other._ptr[i]) {
                    return true;
                }
                if (_ptr[i] > other._ptr[i]) {
                    return false;
                }
            }
            return _length < other._length;
        }
        inline bool operator>(const LinearList& other) const noexcept {
            return other < *this;
        }
        inline bool operator<=(const LinearList& other) const noexcept {
            return !(*this > other);
        }
        inline bool operator>=(const LinearList& other) const noexcept {
            return !(*this < other);
        }
    };
}