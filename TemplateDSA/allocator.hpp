#pragma once
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <utility>

namespace sxl {
	template <class Type>
	class Allocator {
	public:
		using value_type = Type;
		using pointer = Type*;
		using const_pointer = const Type*;
		using reference = Type&;
		using const_reference = const Type&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		template <typename U>
		struct rebind {
			using other = Allocator<U>;
		};
		constexpr Allocator() noexcept {}
		constexpr Allocator(const Allocator&) noexcept = default;
		template <class Other>
		constexpr Allocator(const Allocator<Other>&) noexcept {}
		~Allocator() = default;
		Allocator& operator=(const Allocator&) = default;
		pointer allocate(const size_type count) {
			if (count == 0) {
				return nullptr;
			}
			return static_cast<pointer>(std::malloc(count * sizeof(Type)));
		}
		void deallocate(pointer const ptr, const size_type count) {
			std::free(ptr);
		}
		template<class Object, class... Args>
		void construct(Object* ptr, Args&&... args) {
			new (ptr) Object(std::forward<Args>(args)...);
		}
		template<class Object>
		void destroy(Object* const ptr) {
			ptr->~Object();
		}
		size_type max_size() const noexcept {
			return static_cast<size_type>(-1) / sizeof(Type);
		}
		bool operator==(const Allocator&) const noexcept {
			return true;
		}
		bool operator!=(const Allocator&) const noexcept {
			return false;
		}
	};
}