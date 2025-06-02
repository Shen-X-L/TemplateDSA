#pragma once
#include "TheTreeHead.h"
#include "BinaryTree.hpp"

template<typename T, typename Compare = std::less<T>>
class AVLTree {
public:
	struct Node {
		size_t depth;
		T elem;
		Node(const T& inElem = T(), size_t inDepth = 1) : depth(inDepth), elem(inElem) {}
		Node(T&& inElem, size_t inDepth = 1) noexcept : depth(inDepth), elem(inElem) {}
		Node(const Node& other) : depth(other.depth), elem(other.elem) {}
		Node(Node&& other) noexcept : depth(other.depth), elem(std::move(other.elem)) {}
		~Node() = default;
		Node& operator=(const Node& other) {
			if (this != &other) {
				depth = other.depth;
				elem = other.elem;
			}
			return *this;
		}
		Node& operator=(Node&& other) noexcept {
			if (this != &other) {
				depth = other.depth;
				elem = std::move(other.elem);
			}
			return *this;
		}
	};
	using BTree = BinaryTree<Node>;
protected:
	BTree* tree = nullptr;
	Compare compare = Compare();//compare(A,B) A<B=true A=B=false A>B=false
	/*
	* 非递归的更新深度
	* @param node 当前节点
	*/
	inline static void updateDepth(BTree*& node) {
		if (node->lChild == nullptr && node->rChild == nullptr) {
			node->element.depth = 1;
		}
		else if (node->lChild == nullptr) {
			node->element.depth = node->rChild->element.depth + 1;
		}
		else if (node->rChild == nullptr) {
			node->element.depth = node->lChild->element.depth + 1;
		}
		else {
			node->element.depth = node->lChild->element.depth > node->rChild->element.depth
				? node->lChild->element.depth + 1
				: node->rChild->element.depth + 1;
		}
	}
	inline static int factor(BTree*& node) {
		if (node->lChild == nullptr && node->rChild == nullptr) {
			return 0;
		}
		else if (node->lChild == nullptr) {
			return (int)node->rChild->element.depth;
		}
		else if (node->rChild == nullptr) {
			return -(int)node->lChild->element.depth;
		}
		else {
			return (int)(node->lChild->element.depth - node->rChild->element.depth);
		}
	}
	static void balance(BTree*& node) {
		if (factor(node) > 1) {
			if (factor(node->lChild) >= 0) {
				/*
						A					B
					  /   \				  /   \
					B		T3			T1		A
					| \						  / |
					T1	T2					T2	T3

					T1 == T2 + 1 == T3 + 1 == h
					A(T1 + 2, T1 + 1 - T3 == 2)
					B(T1 + 1, T1 - T2 == 1)

					A(T3 + 1, T2 - T3 == 0)
					B(T1 + 1, T1 - (T3 + 1) == 0)
				*/
				BTree::rotateRight(node);
			}
			else {
				/*
						A				A			C
					  /   \			  /   \		  /   \
					B		T4		C	   T4	B		A
					| \				| \	 		| \		| \
					T1	C			B  T3		T1 T2   T3 T4
						| \			| \
						T2 T3		T1 T2

					(T1 == T4 == h)	(T2 == T3 + 1 == h) || (T2 + 1 == T3 == h) || (T2 == T3 == h)
					A(C + 2, (C + 1) - T4 == 2)
					B(C + 1, T1 - C == -1)
					C(max(T2, T3) + 1 == h + 1, T2 - T3 == {-1, 0, 1})

					A(T4 + 1, T3 - T4 == {-1, 0})
					B(T1 + 1, T1 - T2 == {0, 1})
					C(T1 + 2, T1 - T4 = 0)
				*/
				BTree::rotateLeft(node->lChild);
				BTree::rotateRight(node);
			}
		}
		else if (factor(node) < -1) {
			if (factor(node->right) <= 0) {
				/*
						A					B
					  /   \				  /   \
					T1		B			A		T3
						  / |			| \
						T2	T3			T1	T2

					T1 + 1 == T2 + 1 == T3 == h
					A(T3 + 2, T1 - (T3 + 1) == -2)
					B(T3 + 1, T2 - T3 == -1)

					A(T1 + 1, T1 - T2 == 0)
					B(T3 + 1, (T1 + 1) - T3 == 0)
				*/
				BTree::rotateLeft(node);
			}
			else {
				/*
						A				A				C
					  /   \			  /   \			  /   \
					T1		B		T1	   C		A		B
						  / |			 / |		| \		| \
						C	T4			T2 B		T1 T2   T3 T4
					  / |				 / |
					T2 T3				T3 T4

					(T1 == T4 == h)	(T2 == T3 + 1 == h) || (T2 + 1 == T3 == h) || (T2 == T3 == h)
					A(C + 2, T1 - (C + 1) == -2)
					B(C + 1, C - T4 == 1)
					C(max(T2, T3) + 1 == h + 1, T2 - T3 == {-1, 0, 1})

					A(T1 + 1, T1 - T2 == {0, 1})
					B(T4 + 1, T3 - T4 == {-1, 0})
					C(T4 + 2, T1 - T4 = 0)
				*/
				BTree::rotateRight(node->rChild);
				BTree::rotateLeft(node);
			}
		}
		updateDepth(node->lChild);
		updateDepth(node->rChild);
		updateDepth(node);
	}
	static void balanceLeftRecursive(BTree*& node) {
		if (node == nullptr) {
			return;
		}
		balanceLeftRecursive(node->lChild);
		updateDepth(node);
		balance(node);
		return;
	}
public:
	AVLTree() {};
	AVLTree(const Compare& in) :compare(in) {};
	//序列化
	AVLTree(std::initializer_list<T> in, const Compare& otherCompare = Compare()) :compare(otherCompare) {
		for (const T* it = in.begin(); it != in.end(); ++it) {
			insert(*it);
		}
	};
	//拷贝
	AVLTree(const AVLTree& other) :tree(new BTree(*other.tree)), compare(other.compare) {}
	//移动	
	AVLTree(AVLTree&& other)noexcept :tree(other.tree), compare(std::move(other.compare)) {
		other.tree = nullptr;
	}
	//清空所有子节点
	~AVLTree() {
		delete tree;
		tree = nullptr;
	}
	//拷贝赋值函数
	AVLTree& operator=(const AVLTree& other) {
		if (this != &other) {
			delete tree;
			tree = new BTree(*other.tree);
			compare = other.compare;
		}
		return *this;
	}
	//移动赋值函数
	AVLTree& operator=(AVLTree&& other)noexcept {
		if (this != &other) {
			delete tree;
			tree = other.tree;
			compare = std::move(other.compare);
			other.tree = nullptr;
		}
		return *this;
	}
	//清除内容
	void clear() {
		delete tree;
		tree = nullptr;
	};
	inline bool insert(T&& value) {
		if (empty()) {
			tree = new BTree(std::forward<T>(value));
			return true;
		}
		else {
			return insert(tree, std::forward<T>(value));
		}
	}
	static bool insert(BTree*& node, T&& value) {
		if (compare(value, node->element.elem)) {// value < node
			if (node->lChild == nullptr) {
				node->lChild = new BTree(std::forward<T>(value));
				updateDepth(node);
				return true;
			}
			else {
				if (insert(node->lChild, std::forward<T>(value))) {
					updateDepth(node);
					balance(node);
					return true;
				}
				else { return false; }
			}
		}
		else if (compare(node->element.elem, value)) {// node < value
			if (node->rChild == nullptr) {
				node->rChild = new BTree(std::forward<T>(value));
				updateDepth(node);
				return true;
			}
			else {
				if (insert(node->rChild, std::forward<T>(value))) {
					updateDepth(node);
					balance(node);
					return true;
				}
				else { return false; }
			}
		}
		else {
			return false;
		};// value == node
	};
	//擦除元素
	static bool erase(BTree*& node, const T& value) {
		if (compare(value, node->element.elem)) {
			if (node->lChild != nullptr && erase(node->lChild, value)) {
				updateDepth(node);
				balance(node);
				return true;
			}
			else { return false; }
		}
		else if (compare(node->element.elem, value)) {
			if (node->rChild != nullptr && erase(node->rChild, value)) {
				updateDepth(node);
				balance(node);
				return true;
			}
			else { return false; }
		}
		if (node != nullptr) {
			deleteNode(node);
			return true;
		}
		return false;
	};
	static void deleteNode(BTree*& node) {
		if (node == nullptr) {//没有找到目标节点
			return;
		}
		else if (node->lChild == nullptr && node->rChild == nullptr) {//目标节点是叶子节点
			delete node;
			node = nullptr;
		}
		else if (node->lChild == nullptr || node->rChild == nullptr) {//目标节点只有一个子节点
			BTree* childPtr = (node->lChild != nullptr) ? node->lChild : node->rChild;
			node->lChild = nullptr;
			node->rChild = nullptr;
			delete node;
			node = childPtr;
		}
		else if (node->rChild->lChild == nullptr) {
			BTree* successor = node->rChild;
			node->element = std::move(successor->element);
			node->rChild = successor->rChild;
			successor->rChild = nullptr;
			delete successor;
			updateDepth(node);
			balance(node);

		}
		else {//目标节点有两个子节点
			BTree* successor = node->rChild;
			BTree* parent = nullptr;
			while (successor->lChild != nullptr) {
				parent = successor;
				successor = successor->lChild;
			}
			node->element = std::move(successor->element);
			parent->lChild = successor->rChild;
			successor->rChild = nullptr;
			delete successor;
			balanceLeftRecursive(node->rChild);
		}
		return;
	}
	//寻找元素
	T& find(const T& value) {
		BTree* temp = _getNode(value);
		if (temp != nullptr) { return temp->element.elem; }
		else { return nullRef<T>(); };
	}
	//寻找元素 返回树节点指针
	BTree* _getNode(const T& value) {
		BTree* temp = tree;
		while (temp != nullptr) {
			if (compare(value, temp->element.elem)) { temp = temp->lChild; }//value < temp
			else if (compare(temp->element.elem, value)) { temp = temp->rChild; }// value > temp
			else { return temp; }// value == temp
		}
		return nullptr;
	}
	//寻找大于value的元素下界 返回节点指针
	template<template<typename>class Stack>
	BTree* _getHigherNode(const T& value) {
		Stack<BTree*> stack;
		BTree* temp = tree;
		while (temp != nullptr) {
			if (compare(value, temp->element.elem)) {//value < temp
				if (temp->lChild != nullptr) {
					stack.push(temp);
					temp = temp->lChild;
				}
				else { return temp; }
			}
			else {// value >= temp
				if (temp->rChild != nullptr) {
					stack.push(temp);
					temp = temp->rChild;
				}
				else {
					BTree* parent = nullptr;
					while (!stack.empty()) {
						parent = stack.top();
						stack.pop();
						if (temp == parent->rChild) { temp = parent; }//value >= temp > parent
						else { return parent; }//parent > value >= temp
					}
					return nullptr;
				}
			}
		}
		return nullptr;
	}
	//寻找小于value的元素上界 返回节点指针
	template<template<typename>class Stack>
	BTree* _getLowerNode(const T& value) {
		Stack<BTree*> stack;
		BTree* temp = tree;
		while (temp != nullptr) {
			if (compare(temp->element.elem, value)) {//value > temp
				if (temp->rChild != nullptr) {
					stack.push(temp);
					temp = temp->rChild;
				}
				else { return temp; }
			}
			else {// value <= temp
				if (temp->lChild != nullptr) {
					stack.push(temp);
					temp = temp->lChild;
				}
				else {
					BTree* parent = nullptr;
					while (!stack.empty()) {
						parent = stack.top();
						stack.pop();
						if (temp == parent->lChild) { temp = parent; }//value <= temp < parent
						else { return parent; }//parent < value <= temp
					}
					return nullptr;
				}
			}
		}
		return nullptr;
	}
	//树是否为空
	inline bool empty() const {
		return tree == nullptr;
	}
	//元素数量
	inline size_t size() const {
		if (empty())
			return 0;
		return tree->sizeRecursive();
	}
	//最大元素
	T& maxElem() const {
		if (empty())
			return nullRef<T>();
		BTree* temp = tree;
		while (temp->rChild != nullptr) { temp = temp->rChild; }
		return temp->element.elem;
	}
	//最小元素
	T& minElem() const {
		if (empty())
			return nullRef<T>();
		BTree* temp = tree;
		while (temp->lChild != nullptr) { temp = temp->lChild; }
		return temp->element.elem;
	}
	//主动计算重新深度
	static void updateDepthRecursive(BTree*& node) {
		if (node == nullptr) {
			return;
		}
		updateDepthRecursive(node->lChild);
		updateDepthRecursive(node->rChild);
		updateDepth(node);
		return;
	}
	//主动平衡
	static void balanceRecursive(BTree*& node) {
		if (node == nullptr) {
			return;
		}
		balanceRecursive(node->lChild);
		balanceRecursive(node->rChild);
		updateDepth(node);
		balance(node);
	}
	//检查树是否存在元素不符合排序
	template<template<typename>class Stack>
	void correctOrder() {
		if (empty())
			return;
		Stack<BTree*> stack;//errorElem储存错误元素stack用于树的遍历
		Stack<T> errorElem;
		BTree* ptr = tree;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				if ((ptr->lChild == nullptr || (compare(ptr->lChild->element.elem, ptr->element.elem)))//lchild为空或lchild<ptr 并且 rchild为空或ptr<rchild
					&& (ptr->rChild == nullptr || (compare(ptr->element.elem, ptr->rChild->element.elem)))) {
					stack.push(ptr->rChild); // 右子树入栈
					ptr = ptr->lChild; // 访问左节点
				}
				else {
					if (ptr->lChild != nullptr && (!compare(ptr->lChild->element.elem, ptr->element.elem))) {//lchild>=ptr
						errorElem.push(std::move(ptr->lChild->element.elem));
						erase(ptr->lChild->element.elem);
					}
					if (ptr->rChild != nullptr && (!compare(ptr->element.elem, ptr->rChild->element.elem))) {//ptr>=rchild
						errorElem.push(std::move(ptr->rChild->element.elem));
						erase(ptr->rChild->element.elem);
					}
				}
			}
			else {//空节点,回退栈
				ptr = stack.top();
				stack.pop();
			}
		}
		while (!errorElem.empty()) {
			insert(std::move(errorElem.top()));
			errorElem.pop();
		}
		return;
	}
	//中序遍历
	void inOrderTraverse(typename BTree::Function func) {
		if (tree != nullptr) { tree->_inOrderRecursiveWithFunctionTraverse(func); }
		return;
	}
};