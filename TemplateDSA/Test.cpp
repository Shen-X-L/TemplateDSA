#include <iostream>
#include <forward_list>
#include "LinkedList/TailLinkedList.hpp"
#include "LinkedList/LinkedList.hpp"


int main() {
	TailLinkedList<int> a = {1,2,3,4,5,6,7,8,9};
	a.cut(9);
	for (auto temp : a) {
		std::cout << temp << " ";
	}
	LinkedList<int>b;
}


