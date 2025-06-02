#include <iostream>
#include <forward_list>
#include "LinkedList/TailLinkedList.hpp"
#include "LinkedList/LinkedList.hpp"


int main() {
	TailLinkedList<int> a = {1,2,3,4,5,6,7,8,9};
	a.cut(6);
	for (auto temp : a) {
		std::cout << temp << " ";
	}
	std::cout << std::endl;
	LinkedList<int>b = { 1,2,3,4,5,6,7,8,9 };
	b.cut(6);
	b.emplace(2, 9);
	for (auto temp : b) {
		std::cout << temp << " ";
	}
	std::cout << std::endl;
	auto it = a.before_begin();
	++it;
	a.splice_after(it, b.cbegin(), b.cend());
	for (auto temp : a) {
		std::cout << temp << " ";
	}
	TailLinkedList<int> d = TailLinkedList<int>(a.cbegin(), a.cend());
}


