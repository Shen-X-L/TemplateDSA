#pragma once
#include <cstddef>          //size_t
#include <iterator>         //std::reverse_iterator
#include <stdexcept>        //std::out_of_range
#include <initializer_list> //std::initializer_list<>
#include <type_traits>      //std::is_nothrow_move_assignable_v<>

template<typename T, class Alloc = std::allocator<T>>//���Ա�
class LinearList {
public:
	using value_type = T;
	using allocator_type = Alloc;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using iterator = T*;
	using const_iterator = const T*;
	using reverse_iterator = std::reverse_iterator<T*>;
	using const_reverse_iterator = std::reverse_iterator<const T*>;
protected:
	pointer _ptr;
	size_t _length;//Ԫ�ظ���
	size_t _capacity;//�����ڴ泤�� 
	Alloc _alloc;

public:
	LinearList() noexcept :_ptr(nullptr), _length(0), _capacity(0) {}
	//����size��Ԫ��
	explicit LinearList(size_t n, const Alloc& alloc = Alloc()) : _alloc(alloc) {
		_ptr = _alloc.allocate(n);
		_capacity = _length = n;
		for (size_t i = 0; i < n; ++i) {
			_alloc.construct(&_ptr[i]);
		}
	}
	//����size��inԪ��
	LinearList(size_t n, const T& value, const Alloc& alloc = Alloc())
		:_alloc(alloc) {
		_ptr = _alloc.allocate(n);
		_capacity = _length = n;
		for (size_t i = 0; i < n; ++i) {
			_alloc.construct(&_ptr[i], value);
		}
	}
	//�б��ʼ��
	LinearList(std::initializer_list<T> ilist, const Alloc& alloc = Alloc())
		:LinearList(ilist.begin(), ilist.end(), alloc) {
	}
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
		} else {
			_ptr = _alloc.allocate(_length);
			for (size_t i = 0; i < _length; ++i) {
				_alloc.construct(&_ptr[i], other._ptr[i]);
			}
		}
	}
	//����������������
	LinearList(const LinearList& other, const Alloc& alloc)
		:_length(other._length), _capacity(other._length), _alloc(alloc) {
		if (_length == 0) {
			_ptr = nullptr;
		} else {
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
	LinearList& operator=(const LinearList& other) {
		if (this != &other) {
			LinearList tmp(other);  // �������죨�������쳣������Ӱ�� this��
			this->swap(tmp);        // noexcept ����
		}
		return *this;
	}
	//�ƶ���ֵ
	LinearList& operator=(LinearList&& other) noexcept {
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
	LinearList& operator=(std::initializer_list<T> ilist) {
		assign(ilist.begin(), ilist.end());
	}
	//�������Ա���
	inline size_t size() const noexcept { return _length; }
	//������Ա�Ϊ�� ����TRUE
	inline bool empty() const noexcept { return _length == 0; }
	//�����ڴ泤�� 
	inline size_t capacity() const noexcept { return _capacity; }
	//��ձ�
	void clear() {
		for (size_t i = 0; i < _length; ++i) {
			_alloc.destroy(&_ptr[i]);
		}
		_length = 0;
		return;
	}
	//������Ԫ��
	static constexpr size_t max_size()noexcept { return (static_cast<size_t>(-1)) / sizeof(T); }
	//�ӳ���ض�����
	void resize(size_t newLen) {
		if (newLen < _length) {
			//�ضϲ�����
			for (size_t i = newLen; i < _length; ++i) {
				_alloc.destroy(&_ptr[i]);
			}
		} else if (newLen > _length) {
			reserve(newLen);
			for (size_t i = _length; i < newLen; ++i) {
				_alloc.construct(&_ptr[i]);
			}
		}
		_length = newLen;
		return;
	}
	//�ӳ���ֵ��ض�����
	void resize(size_t newLen, const T& value) {
		if (newLen < _length) {
			//�ضϲ�����
			for (size_t i = newLen; i < _length; ++i) {
				_alloc.destroy(&_ptr[i]);
			}
		} else if (newLen > _length) {
			reserve(newLen);
			for (size_t i = _length; i < newLen; ++i) {
				_alloc.construct(&_ptr[i], value);
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
		T* newPtr = _alloc.allocate(newCap);
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
			T* newPtr = _alloc.allocate(newCap);
			for (size_t i = 0; i < _length; ++i) {
				_alloc.construct(&newPtr[i], std::move_if_noexcept(_ptr[i]));
			}
			for (size_t i = 0; i < _length; ++i) {
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
	Alloc get_allocator() const noexcept { return _alloc; }
	//����Ԫ������
	T& operator[](size_t i) noexcept { return _ptr[i]; }
	const T& operator[](size_t i) const noexcept { return _ptr[i]; }
	T& at(size_t i) {
		if (i >= _length) {
			throw std::out_of_range("Index out of range LinearList::at");
		}
		return _ptr[i];
	}
	const T& at(size_t i) const {
		if (i >= _length) {
			throw std::out_of_range("Index out of range LinearList::at");
		}
		return _ptr[i];
	}
	//����Ԫ��ָ��
	T* _atPtr(size_t i) {
		if (i >= _length) {
			throw std::out_of_range("Index out of range LinearList::_atPtr");
		}
		return _ptr + i;
	}
	const T* _atPtr(size_t i) const {
		if (i >= _length) {
			throw std::out_of_range("Index out of range LinearList::_atPtr");
		}
		return _ptr + i;
	}
	//������λ
	T& front()noexcept { return _ptr[0]; }
	const T& front()const noexcept { return _ptr[0]; }
	//����ĩλ
	T& back()noexcept { return _ptr[_length - 1]; }
	const T& back()const noexcept { return _ptr[_length - 1]; }
	//��������ָ��
	T* data() noexcept { return _ptr; }
	const T* data()const noexcept { return _ptr; }
	const T* cdata()const noexcept { return _ptr; }
	//������
	T* begin() noexcept { return _ptr; }
	T* end() noexcept { return _ptr + _length; }
	const T* begin() const noexcept { return _ptr; }
	const T* end() const noexcept { return _ptr + _length; }
	const T* cbegin() const noexcept { return _ptr; }
	const T* cend() const noexcept { return _ptr + _length; }
	//���������
	std::reverse_iterator<T*> rbegin() noexcept {
		return std::make_reverse_iterator(_ptr + _length);
	}
	std::reverse_iterator<T*> rend() noexcept {
		return std::make_reverse_iterator(_ptr);
	}
	std::reverse_iterator<const T*> rbegin() const noexcept {
		return std::make_reverse_iterator(_ptr + _length);
	}
	std::reverse_iterator<const T*> rend() const noexcept {
		return std::make_reverse_iterator(_ptr);
	}
	std::reverse_iterator<const T*> crbegin() const noexcept {
		return std::make_reverse_iterator(_ptr + _length);
	}
	std::reverse_iterator<const T*> crend() const noexcept {
		return std::make_reverse_iterator(_ptr);
	}
	//��ָ��λ��ǰ����Ԫ��
	template <class ...Args>
	T* emplace(const T* it, Args &&...args) {
		size_t j = it - _ptr;
		reserve(_length + 1);
		for (size_t i = _length; i > j; --i) {
			_alloc.construct(&_ptr[i], std::move(_ptr[i - 1]));
			_alloc.destroy(&_ptr[i - 1]);
		}
		_alloc.construct(&_ptr[j], std::forward<Args>(args)...);
		++_length;
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
		_alloc.construct(&_ptr[index], std::forward<Args>(args)...);
		++_length;
		return index;
	}
	//��ָ��λ�ò���Ԫ��
	T* insert(const T* it, const T& value) {
		size_t j = it - _ptr;
		reserve(_length + 1);
		for (size_t i = _length; i > j; --i) {
			_alloc.construct(&_ptr[i], std::move(_ptr[i - 1]));
			_alloc.destroy(&_ptr[i - 1]);
		}
		_alloc.construct(&_ptr[j], value);
		++_length;
		return _ptr + j;
	}
	size_t insert(size_t index, const T& value) {
		if (index > _length) {
			throw std::out_of_range("Index out of range LinearList::insert");
			return;
		}
		reserve(_length + 1);
		for (size_t i = _length; i > index; --i) {
			_alloc.construct(&_ptr[i], std::move(_ptr[i - 1]));
			_alloc.destroy(&_ptr[i - 1]);
		}
		_alloc.construct(&_ptr[index], value);
		++_length;
		return index;
	}
	T* insert(const T* it, T&& value) {
		size_t j = it - _ptr;
		reserve(_length + 1);
		for (size_t i = _length; i > j; --i) {
			_alloc.construct(&_ptr[i], std::move(_ptr[i - 1]));
			_alloc.destroy(&_ptr[i - 1]);
		}
		_alloc.construct(&_ptr[j], std::move(value));
		++_length;
		return _ptr + j;
	}
	size_t insert(size_t index, T&& value) {
		if (index > _length) {
			throw std::out_of_range("Index out of range LinearList::insert");
			return;
		}
		reserve(_length + 1);
		for (size_t i = _length; i > index; --i) {
			_alloc.construct(&_ptr[i], std::move(_ptr[i - 1]));
			_alloc.destroy(&_ptr[i - 1]);
		}
		_alloc.construct(&_ptr[index], std::move(value));
		++_length;
		return index;
	}
	T* insert(const T* it, size_t n, const T& value) {
		size_t j = it - _ptr;
		if (n == 0)
			return const_cast<T*>(it);
		reserve(_length + n);
		for (size_t i = _length; i > j; --i) {
			_alloc.construct(&_ptr[i + n - 1], std::move(_ptr[i - 1]));
			_alloc.destroy(&_ptr[i - 1]);
		}
		for (size_t i = j; i < j + n; ++i) {
			_alloc.construct(&_ptr[i], value);
		}
		_length = _length + n;
		return _ptr + j;
	}
	size_t* insert(size_t index, size_t n, const T& value) {
		if (n == 0)
			return index;
		reserve(_length + n);
		for (size_t i = _length; i > index; --i) {
			_alloc.construct(&_ptr[i + n - 1], std::move(_ptr[i - 1]));
			_alloc.destroy(&_ptr[i - 1]);
		}
		for (size_t i = index; i < index + n; ++i) {
			_alloc.construct(&_ptr[i], value);
		}
		_length = _length + n;
		return index;
	}
	template <typename InputIt>
	T* insert(const T* it, InputIt first, InputIt last) {
		size_t j = it - _ptr;
		size_t n = last - first;
		if (n == 0)
			return const_cast<T*>(it);
		reserve(_length + n);
		for (size_t i = _length; i > j; --i) {
			_alloc.construct(&_ptr[i + n - 1], std::move(_ptr[i - 1]));
			_alloc.destroy(&_ptr[i - 1]);
		}
		for (size_t i = j; i < j + n; ++i) {
			_alloc.construct(&_ptr[i], *first);
			++first;
		}
		_length = _length + n;
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
		for (size_t i = index; i < index + n; ++i) {
			_alloc.construct(&_ptr[i], *first);
			++first;
		}
		_length = _length + n;
		return index;
	}
	T* insert(const T* it, std::initializer_list<T> ilist) {
		return insert(it, ilist.begin(), ilist.end());
	}
	size_t insert(size_t index, std::initializer_list<T> ilist) {
		return insert(index, ilist.begin(), ilist.end());
	}
	//�Ƴ�ָ��λ�õ�Ԫ��
	T* erase(const T* it) noexcept(std::is_nothrow_move_assignable_v<T>) {
		size_t n = it - _ptr;
		for (size_t i = n; i < _length - 1; ++i) {
			_ptr[i] = std::move(_ptr[i + 1]);
		}
		_alloc.destroy(&_ptr[_length]);
		--_length;
		return const_cast<T*>(it); //����һ��ָ��ɾ��Ԫ��֮��ĵ�����
	}
	size_t erase(const size_t index) noexcept(std::is_nothrow_move_assignable_v<T>) {
		for (size_t i = index; i < _length - 1; ++i) {
			_ptr[i] = std::move(_ptr[i + 1]);
		}
		_alloc.destroy(&_ptr[_length]);
		--_length;
		return const_cast<size_t>(index);
	}
	//�Ƴ�ָ����Χ��Ԫ��
	T* erase(const T* first, const T* last) noexcept(std::is_nothrow_move_assignable_v<T>) {
		size_t n = last - first;
		for (size_t i = first - _ptr; i < _length - n; ++i) {
			_ptr[i] = std::move(_ptr[i + n]);
		}
		for (size_t i = _length - n; i < _length; ++i) {
			_alloc.destroy(&_ptr[i]);
		}
		_length = _length - n;
		return const_cast<T*>(first);
	}
	size_t erase(const size_t first, const size_t last) noexcept(std::is_nothrow_move_assignable_v<T>) {
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
	template <class ...Args>
	T* emplace_back(Args &&...args) {
		if (_length >= _capacity) {
			reserve(_length + 1);
		}
		_alloc.construct(&_ptr[_length], std::forward<Args>(args)...);
		++_length;
		return _ptr + j;
	}
	//ĩβ����Ԫ��
	void push_back(const T& value) {
		if (_length >= _capacity) {
			reserve(_length + 1);
		}
		_alloc.construct(&_ptr[_length], value);
		++_length;
	}
	void push_back(T&& value) {
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
	void pop_back() noexcept {
		_alloc.destroy(&_ptr[_length]);
		--_length;
	}
	//�ع���
	void assign(size_t n, const T& value) {
		clear();
		reserve(n);
		_length = n;
		for (size_t i = 0; i < n; ++i) {
			_alloc.construct(&_ptr[i], value);
		}
	}
	template <typename InputIt>
	void assign(InputIt first, InputIt last) {
		clear();
		size_t n = last - first;
		reserve(n);
		_length = n;
		for (size_t i = 0; i < n; ++i, ++first) {
			_alloc.construct(&_ptr[i], *first);
		}
	}
	void assign(std::initializer_list<T> ilist) {
		assign(ilist.begin(), ilist.end());
	}
	//����
	void swap(LinearList& other) noexcept {
		T* temp_ptr = _ptr;
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
