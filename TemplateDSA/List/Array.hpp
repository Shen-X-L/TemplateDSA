#pragma once
#include <cstddef>           //size_t
#include <iterator>         //std::reverse_iterator
#include <stdexcept>        //std::out_of_range
#include <initializer_list> //std::initializer_list<>
#include "../allocator.hpp"

namespace sxl {
    template <class Type, size_t N>
    class Array {
    public:
        using value_type = Type;
        using pointer = Type*;
        using const_pointer = const Type*;
        using reference = Type&;
        using const_reference = const Type&;
        using iterator = Type*;
        using const_iterator = const Type*;
        using reverse_iterator = std::reverse_iterator<Type*>;
        using const_reverse_iterator = std::reverse_iterator<const Type*>;
        Type _elements[N];
    public:
        Type& operator[](size_t index) noexcept {
            return _elements[index];
        }
        const Type& operator[](size_t index) const noexcept {
            return _elements[index];
        }
        Type& at(size_t index) {
            if (index >= N) 
                throw std::out_of_range("Index out of range Array::at");
                return _elements[index];
        }
        const Type& at(size_t index) const {
            if (index >= N) 
                throw std::out_of_range("Index out of range Array::at");
                return _elements[index];
        }
        void fill(const Type& value) noexcept{
            for (size_t i = 0; i < N; ++i) {
                _elements[i] = value;
            }
        }
        void swap(Array& other) noexcept{
            for (size_t i = 0; i < N; ++i) {
                Type temp = std::move(_elements[i]);
                _elements[i] = std::move(other._elements[i]);
                other._elements[i] = std::move(temp);
            }
        }
        Type& front() noexcept {
            return _elements[0];
        }
        const Type& front() const noexcept {
            return _elements[0];
        }
        Type& back() noexcept {
            return _elements[N - 1];
        }
        const Type& back() const noexcept {
            return _elements[N - 1];
        }
        static constexpr bool empty() noexcept {
            return false;
        }
        static constexpr size_t size() noexcept {
            return N;
        }
        static constexpr size_t max_size() noexcept {
            return N;
        }
        Type* data() noexcept {
            return _elements;
        }
        const Type* data() const noexcept {
            return _elements;
        }  
        const Type* cdata() const noexcept {
            return _elements;
        }
        Type* begin() noexcept {
            return _elements;
        }
        Type* end() noexcept {
            return _elements + N;
        }
        const Type* begin() const noexcept {
            return _elements;
        }
        const Type* end() const noexcept {
            return _elements + N;
        }
        const Type* cbegin() const noexcept {
            return _elements;
        }
        const Type* cend() const noexcept {
            return _elements + N;
        }
        std::reverse_iterator<Type*> rbegin() noexcept {
            return std::make_reverse_iterator(_elements + N);
        }
        std::reverse_iterator<Type*> rend() noexcept {
            return std::make_reverse_iterator(_elements);
        }
        std::reverse_iterator<const Type*> rbegin() const noexcept {
            return std::make_reverse_iterator(_elements + N);
        }
        std::reverse_iterator<const Type*> rend() const noexcept {
            return std::make_reverse_iterator(_elements);
        }
        std::reverse_iterator<const Type*> crbegin() const noexcept {
            return std::make_reverse_iterator(_elements + N);
        }
        std::reverse_iterator<const Type*> crend() const noexcept {
            return std::make_reverse_iterator(_elements);
        }
    };
    template <class Type>
    class Array<Type, 0> {
    public:
        using value_type = Type;
        using pointer = Type*;
        using const_pointer = const Type*;
        using reference = Type&;
        using const_reference = const Type&;
        using iterator = Type*;
        using const_iterator = const Type*;
        using reverse_iterator = Type*;
        using const_reverse_iterator = const Type*;
        Type& operator[](size_t index) noexcept {
            return *nullptr;
        }
        const Type& operator[](size_t index) const noexcept {
            return *nullptr;
        }
        Type& at(size_t index) {
            throw std::out_of_range("Index out of range Array<0>::at");
        }
        Type const& at(size_t index) const {
            throw std::out_of_range("Index out of range Array<0>::at");
        }
        void fill(const Type&) noexcept {
        }
        void swap(Array&) noexcept {
        }
        Type& front() noexcept {
            return *nullptr;
        }
        const Type& front() const noexcept {
            return *nullptr;
        }
        Type& back() noexcept {
            return *nullptr;
        }
        const Type& back() const noexcept {
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
        Type* data() noexcept {
            return nullptr;
        }
        const Type* data() const noexcept {
            return nullptr;
        }
        const Type* cdata() const noexcept {
            return nullptr;
        }
        Type* begin() noexcept {
            return nullptr;
        }
        Type* end() noexcept {
            return nullptr;
        }
        const Type* begin() const noexcept {
            return nullptr;
        }
        const Type* end() const noexcept {
            return nullptr;
        }
        const Type* cbegin() const noexcept {
            return nullptr;
        }
        const Type* cend() const noexcept {
            return nullptr;
        }
        Type* rbegin() noexcept {
            return nullptr;
        }
        Type* rend() noexcept {
            return nullptr;
        }
        const Type* rbegin() const noexcept {
            return nullptr;
        }
        const Type* rend() const noexcept {
            return nullptr;
        }
        const Type* crbegin() const noexcept {
            return nullptr;
        }
        const Type* crend() const noexcept {
            return nullptr;
        }
    };
}
