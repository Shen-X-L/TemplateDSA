#pragma once

#include <functional>

template<typename T>
struct TreeNodeOrder{
	T element;
	bool isEmpty = true;
	TreeNodeOrder() {};
	TreeNodeOrder(const T& other, bool empty) :element(other), isEmpty(empty) {};
};