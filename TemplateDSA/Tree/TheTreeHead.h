#pragma once
#include "myHead.h"
#include "StaticStack.hpp"
#include "StaticQueue.hpp"
#include "LinearList.hpp"

template<typename T>
struct TreeNodeOrder{
	T element;
	bool isEmpty = true;
	TreeNodeOrder() {};
	TreeNodeOrder(const T& other, bool empty) :element(other), isEmpty(empty) {};
};