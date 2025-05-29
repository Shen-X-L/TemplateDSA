#pragma once
#include "TheTreeHead.h"
#include "BinaryTree.hpp"


template<typename T, typename Compare = std::less<T>>
class BinarySortTree {
public:
	using BSTree = BinarySortTree<T, Compare>;
	using BTree = BinaryTree<T>;
protected:
	BTree* tree = nullptr;
	Compare compare = Compare();//compare(A,B) A<B=true A=B=false A>B=false
public:
	BinarySortTree() {};
	BinarySortTree(const Compare& in) :compare(in) {};
	//���л�
	BinarySortTree(std::initializer_list<T> in, const Compare& otherCompare = Compare()) :compare(otherCompare) {
		for (const T* it = in.begin(); it != in.end(); ++it) {
			insert(*it);
		}
	};
	//����
	BinarySortTree(const BSTree& other) :tree(new BTree(*other.tree)), compare(other.compare) {}
	//�ƶ�	
	BinarySortTree(BSTree&& other) :tree(other.tree), compare(std::move(other.compare)) {
		other.tree = nullptr;
	}
	//��������ӽڵ�
	~BinarySortTree() {
		delete tree;
		tree = nullptr;
	}
	//������ֵ����
	BSTree& operator=(const BSTree& other) {
		if (this != &other) {
			delete tree;
			tree = new BTree(*other.tree);
			compare = other.compare;
		}
		return *this;
	}
	//�ƶ���ֵ����
	BSTree& operator=(BSTree&& other)noexcept {
		if (this != &other) {
			delete tree;
			tree = other.tree;
			compare = std::move(other.compare);
			other.tree = nullptr;
		}
		return *this;
	}
	//�������
	void clear() {
		delete tree;
		tree = nullptr;
	};
	//����Ԫ�� true����ɹ� false����Ԫ��
	bool insert(T&& value) {
		BTree** temp = &tree;
		while (*temp != nullptr) {
			if (compare(value, (*temp)->element)) { temp = &((*temp)->lChild); }//value < temp
			else if (compare((*temp)->element, value)) { temp = &((*temp)->rChild); }// value > temp
			else { return false; };// value == temp
		}
		*temp = new BTree(std::forward<T>(value));
		return true;
	};
	//����Ԫ��
	bool erase(const T& value) {
		BTree** temp = &tree;
		while (*temp != nullptr) {
			if (compare(value, (*temp)->element)) { temp = &((*temp)->lChild); }//value < temp
			else if (compare(((*temp)->element), value)) { temp = &((*temp)->rChild); }// value > temp
			else { break; }// value == temp
		}
		if (*temp == nullptr) {
			return false;
		}
		deleteNode(*temp);
		return true;
	};
	static void deleteNode(BTree*& tempPtr) {
		if (tempPtr == nullptr) {//û���ҵ�Ŀ��ڵ�
			return;
		}
		else if (tempPtr->lChild == nullptr && tempPtr->rChild == nullptr) {//Ŀ��ڵ���Ҷ�ӽڵ�
			delete tempPtr;
			tempPtr = nullptr;
		}
		else if (tempPtr->lChild == nullptr || tempPtr->rChild == nullptr) {//Ŀ��ڵ�ֻ��һ���ӽڵ�
			BTree* childPtr = (tempPtr->lChild != nullptr) ? tempPtr->lChild : tempPtr->rChild;
			tempPtr->lChild = nullptr;
			tempPtr->rChild = nullptr;
			delete tempPtr;
			tempPtr = childPtr;
		}
		else {//Ŀ��ڵ��������ӽڵ�
			BTree* successor = tempPtr->rChild;
			BTree* parent = nullptr;
			while (successor->lChild != nullptr) {
				parent = successor;
				successor = successor->lChild;
			}
			tempPtr->element = std::move(successor->element);
			if (parent == nullptr) { tempPtr->rChild = successor->rChild; }
			else { parent->lChild = successor->rChild; }
			successor->rChild = nullptr;
			delete successor;
		}
		return;
	}
	//Ѱ��Ԫ��
	T& find(const T& value) {
		BTree* temp = _getNode(value);
		if (temp != nullptr) { return temp->element; }
		else { return nullRef<T>(); };
	}
	//Ѱ��Ԫ�� �������ڵ�ָ��
	BTree* _getNode(const T& value) {
		BTree* temp = tree;
		while (temp != nullptr) {
			if (compare(value, temp->element)) { temp = temp->lChild; }//value < temp
			else if (compare(temp->element, value)) { temp = temp->rChild; }// value > temp
			else { return temp; }// value == temp
		}
		return nullptr;
	}
	//Ѱ�Ҵ���value��Ԫ���½� ���ؽڵ�ָ��
	template <template<typename>class Stack>
	BTree* _getHigherNode(const T& value) {
		Stack<BTree*> stack;
		BTree* temp = tree;
		while (temp != nullptr) {
			if (compare(value, temp->element)) {//value < temp
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
	//Ѱ��С��value��Ԫ���Ͻ� ���ؽڵ�ָ��
	template <template<typename>class Stack>
	BTree* _getLowerNode(const T& value) {
		Stack<BTree*> stack;
		BTree* temp = tree;
		while (temp != nullptr) {
			if (compare(temp->element, value)) {//value > temp
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
	//���ĸ߶�
	size_t depth()const {
		if (empty())
			return 0;
		return tree->depthRecursive();
	}
	//���Ƿ�Ϊ��
	inline bool empty() const {
		return tree == nullptr;
	}
	//Ԫ������
	inline size_t size() const {
		if (empty())
			return 0;
		return tree->sizeRecursive();
	}
	//���Ԫ��
	T& maxElem() const {
		if (empty())
			return nullRef<T>();
		BTree* temp = tree;
		while (temp->rChild != nullptr) { temp = temp->rChild; }
		return temp->element;
	}
	//��СԪ��
	T& minElem() const {
		if (empty())
			return nullRef<T>();
		BTree* temp = tree;
		while (temp->lChild != nullptr) { temp = temp->lChild; }
		return temp->element;
	}
	//ƽ��
	struct BalanceNode {
		size_t depth = 0;
		int64_t factor = 0;
	};
	static BalanceNode balance(BTree*& root) {
		if (root == nullptr) {
			return{ 0,0 };
		}
		else {
			BalanceNode currentNode = { 1,0 }, leftNode = { 0,0 }, rightNode = { 0,0 };
			if (root->lChild != nullptr) {
				leftNode = balance(root->lChild);
			}
			if (root->rChild != nullptr) {
				rightNode = balance(root->rChild);
			}
			//����ƽ������
			currentNode.factor = leftNode.depth - rightNode.depth;
			if (currentNode.factor > 1) {
				if (leftNode.factor >= 0) {
					/*
						A					B
					  /   \				  /   \
					B		T3			T1		A
					| \						  / |
					T1	T2					T2	T3
					(T1 > T3)	(T1 == T2 || T1 == T2 + 1)
					A(T1 + 2, (T1 + 1) - T3 > 1)
					B(T1 + 1, T1 - T2 == {0, 1})

					A(T2 + 1, T2 - T3 >= 0)
					B(T2 + 2, T1 - (T2 + 1) == {-1, 0})
					*/
					BTree::rotateRight(root);
					return balance(root);
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
						(T1 == max(T2, T3))	(T1 >= T4)
						A(T1 + 3, (T1 + 2) - T4 > 1)
						B(T1 + 2, T1 - (T1 + 1) == -1)
						C(T1 + 1, T2 - T3 == {-1, 0, 1})

						A(max(T3, T4) + 1, T3 - T4 >= -1)
						B(T1 + 1, T1 - T2 = { 0,1 })
						C(T1 + 2, T1 - max(T3, T4) = { 0,1 })
					*/
					BTree::rotateLeft(root->lChild);
					BTree::rotateRight(root);
					return balance(root);
				}
			}
			else if (currentNode.factor < -1) {
				if (rightNode.factor <= 0) {
					/*
						A					B
					  /   \				  /   \
					T1		B			A		T3
						  / |			| \
						T2	T3			T1	T2
					(T1 < T3)	(T2 == T3 || T2 + 1 == T3)
					A(T3 + 2, T1 - (T3 + 1) < 1)
					B(T3 + 1, T2 - T3 == {-1, 0});

					A(T2 + 1, T1 - T2 <= 0)
					B(T2 + 2, (T2 + 1) - T3 == {0, 1})
					*/
					BTree::rotateLeft(root);
					return balance(root);
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
						A(h+1,h-T2=0||1)
						B(h+1,T3-h=0||-1)
						C(h+2,(h+1)-(h+1)=0)
						(T4 == max(T2, T3))	(T1 <= T4)
						A(T4 + 3, T1 - (T4 + 2) < -1)
						B(T4 + 2, (T4 + 1) - T4 == 1)
						C(T4 + 1, T2 - T3 == {-1, 0, 1})

						A(max(T1, T2) + 1, T1 - T2 <= 1)
						B(T4 + 1, T3 - T4 = { -1,0 })
						C(T4 + 2, max(T1, T2) - T4 = { -1,0 })
					*/
					BTree::rotateRight(root->rChild);
					BTree::rotateLeft(root);
					return balance(root);
				}
			}
			else {
				currentNode.depth = leftNode.depth > rightNode.depth ? leftNode.depth + 1 : rightNode.depth + 1;
				return currentNode;
			}
		}
	}
	/*
				a
			b		c
		d	e			f
	   g h i
		ptr=a
	stack=a	ptr=b
	stack=a,b	ptr=d
	stack=a,b,d	ptr=g
	stack=a,b,d,g	ptr=Null
	pv=back()=g	ptr==pv->r	ptr=pop()=g	(null),(null)->(g,1,0)	stack=a,b,d	�ڵ�Stack=(g,1,0)
	pv=back()=d	ptr!=pv->r	ptr=h
	stack=a,b,d,h	ptr=Null
	pv=back()=h	ptr==pv->r	ptr=pop()=h	(null),(null)->(h,1,0)	stack=a,b,d	�ڵ�Stack=(g,1,0),(h,1,0)
	pv=back()=d	ptr==pv->r	ptr=pop()=d	(g,1,0),(h,1,0)->(d,2,0)	stack=a,b	�ڵ�Stack=(d,2,0)
	pv=back()=b	ptr!=pv->r	ptr=e
	stack=a,b,e	ptr=i
	stack=a,b,e,i	ptr=Null
	pv=back()=i	ptr==pv->r	ptr=pop()=i	(null),(null)->(i,1,0)	stack=a,b,e �ڵ�Stack=(d,2,0),(i,1,0)
	pv=back()=e	ptr!=pv->r	ptr=Null
	pv=back()=e	ptr==pv->r	ptr=pop()=e	(i,1,0),(null)->(e,2,1)	stack=a,b	�ڵ�Stack=(d,2,0),(e,2,1)
	pv=back()=b	ptr!=pv->r	ptr=Null
	pv=back()=b	ptr==pv->r	ptr=pop()=b	(d,2,0),(e,2,1)->(b,3,1)	stack=a	�ڵ�Stack=(b,3,1)
	pv=back()=a	ptr==pv->r	ptr=c
	stack=a,c	ptr=Null
	pv=back()=c	ptr!=pv->r	ptr=f
	stack=a,c,f	ptr=Null
	pv=back()=f	ptr==pv->r	ptr=pop()=f	(null),(null)->(f,1,0)	stack=a,c	�ڵ�Stack=(b,3,1),(f,1,0)
	pv=back()=c	ptr==pv->r	ptr=pop()=c	(null),(f,1,0)->(c,2,-1)	stack=a	�ڵ�Stack=(b,3,1),(c,2,-1)
	pv=back()=a	ptr==pv->r	ptr=pop()=a	(b,3,1),(c,2,-1)->(a,4,1)	stack=	�ڵ�Stack=(a,4,1)
	*/
	template <template<typename>class Stack>
	static BalanceNode balance(BTree*& root) {
		Stack<BTree**> ptrStack;
		Stack<BalanceNode> nodeStack;
		BalanceNode currentNode = { 1,0 }, leftNode = { 0,0 }, rightNode = { 0,0 };
		BTree** ptr = &root, ** prev = nullptr;
		while ((*ptr) != nullptr || !ptrStack.empty()) {
			if ((*ptr) != nullptr && (*ptr) != (*prev)) {
				ptrStack.push(ptr);
				ptr = &((*ptr)->lChild);//������ڵ�
			}
			else {//�սڵ�,���˵�ջ���������ҽڵ�
				prev = ptrStack.top();
				if ((*ptr) == (*prev)->rChild) {//�ýڵ����������������,��Ȿ�ڵ�
					ptr = &(*prev);
					if ((*ptr)->rChild != nullptr) {
						rightNode = nodeStack.top();
						nodeStack.pop();
					}
					else {
						rightNode = { 0,0 };
					}
					if ((*ptr)->lChild != nullptr) {
						leftNode = nodeStack.top();
						nodeStack.pop();
					}
					else {
						leftNode = { 0,0 };
					}
					currentNode.factor = leftNode.depth - rightNode.depth;
					if (currentNode.factor > 1) {
						if (leftNode.factor >= 0) {
							/*
								A					B
							  /   \				  /   \
							B		T3			T1		A
							| \						  / |
							T1	T2					T2	T3
							(T1 > T3)	(T1 == T2 || T1 == T2 + 1)
							A(T1 + 2, (T1 + 1) - T3 > 1)
							B(T1 + 1, T1 - T2 == {0, 1})

							A(T2 + 1, T2 - T3 >= 0)
							B(T2 + 2, T1 - (T2 + 1) == {-1, 0})
							*/
							BTree::rotateRight(*ptr);
							nodeStack.push(balance<Stack>(*ptr));
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
								(T1 == max(T2, T3))	(T1 >= T4)
								A(T1 + 3, (T1 + 2) - T4 > 1)
								B(T1 + 2, T1 - (T1 + 1) == -1)
								C(T1 + 1, T2 - T3 == {-1, 0, 1})

								A(max(T3, T4) + 1, T3 - T4 >= -1)
								B(T1 + 1, T1 - T2 = { 0,1 })
								C(T1 + 2, T1 - max(T3, T4) = { 0,1 })
							*/
							BTree::rotateLeft(*ptr->lChild);
							BTree::rotateRight(*ptr);
							nodeStack.push(balance<Stack>(*ptr));
						}
					}
					else if (currentNode.factor < -1) {
						if (rightNode.factor <= 0) {
							/*
								A					B
							  /   \				  /   \
							T1		B			A		T3
								  / |			| \
								T2	T3			T1	T2
							(T1 < T3)	(T2 == T3 || T2 + 1 == T3)
							A(T3 + 2, T1 - (T3 + 1) < 1)
							B(T3 + 1, T2 - T3 == {-1, 0});

							A(T2 + 1, T1 - T2 <= 0)
							B(T2 + 2, (T2 + 1) - T3 == {0, 1})
							*/
							BTree::rotateLeft(*ptr);
							nodeStack.push(balance<Stack>(*ptr));
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
								A(h+1,h-T2=0||1)
								B(h+1,T3-h=0||-1)
								C(h+2,(h+1)-(h+1)=0)
								(T4 == max(T2, T3))	(T1 <= T4)
								A(T4 + 3, T1 - (T4 + 2) < -1)
								B(T4 + 2, (T4 + 1) - T4 == 1)
								C(T4 + 1, T2 - T3 == {-1, 0, 1})

								A(max(T1, T2) + 1, T1 - T2 <= 1)
								B(T4 + 1, T3 - T4 = { -1,0 })
								C(T4 + 2, max(T1, T2) - T4 = { -1,0 })
							*/
							BTree::rotateRight(*ptr->rChild);
							BTree::rotateLeft(*ptr);
							nodeStack.push(balance<Stack>(*ptr));
						}
					}
					else {
						currentNode.depth = leftNode.depth > rightNode.depth ? leftNode.depth + 1 : rightNode.depth + 1;
						nodeStack.push(currentNode);
					}
					ptrStack.pop();//���ڵ������,��ջ
				}
				else
					ptr = &((*ptr)->rChild);
			}
		}
		return currentNode;
	}
	//������Ƿ����Ԫ�ز���������
	template <template<typename>class Stack>
	void correctOrder() {
		if (empty())
			return;
		Stack<BTree*> stack;//errorElem�������Ԫ��stack�������ı���
		Stack<T> errorElem;
		BTree* ptr = tree;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				if ((ptr->lChild == nullptr || (compare(ptr->lChild->element, ptr->element)))//lchildΪ�ջ�lchild<ptr ���� rchildΪ�ջ�ptr<rchild
					&& (ptr->rChild == nullptr || (compare(ptr->element, ptr->rChild->element)))) {
					stack.push(ptr->rChild); // ��������ջ
					ptr = ptr->lChild; // ������ڵ�
				}
				else {
					if (ptr->lChild != nullptr && (!compare(ptr->lChild->element, ptr->element))) {//lchild>=ptr
						errorElem.push(std::move(ptr->lChild->element));
						deleteNode(ptr->lChild);
					}
					if (ptr->rChild != nullptr && (!compare(ptr->element, ptr->rChild->element))) {//ptr>=rchild
						errorElem.push(std::move(ptr->rChild->element));
						deleteNode(ptr->rChild);
					}
				}
			}
			else {//�սڵ�,����ջ
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
	//�������
	void inOrderTraverse(typename BTree::Function func) {
		if (tree != nullptr) { tree->_inOrderRecursiveWithFunctionTraverse(func); }
		return;
	}
};