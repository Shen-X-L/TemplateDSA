#pragma once
#include "TheTreeHead.h"
template<typename T>
class BinaryTree {
public:
	using BTree = BinaryTree<T>;
	template<typename U, typename Compare>
	friend class BinarySortTree;
	template<typename U, typename Compare>
	friend class AVLTree;
protected:
	T element;//����Ԫ��
	BTree* lChild = nullptr;//���ӽڵ� �����ڵ�
	BTree* rChild = nullptr;//���ӽڵ� ����ҽڵ�
public:
	//�޲ι��캯��
	BinaryTree() {}
	//����Ҷ�ӽڵ�
	BinaryTree(const T& in) :element(in) {}
	//���ӽڵ�����
	BinaryTree(const T& left, const T& right)
		:lChild(new BTree(left)), rChild(new BTree(right)) {}
	//���ڵ����ӽڵ�����
	BinaryTree(const T& in, const T& left, const T& right)
		:lChild(new BTree(left)), rChild(new BTree(right)), element(in) {}
	//�ӽڵ�����
	BinaryTree(const T& in, BTree* inLeft, BTree* inRight)
		:element(in), lChild(inLeft), rChild(inRight) {}
	//��ĳ�ֱ�����ʽ������ȫ������
	template <template<typename U>class List, template<typename>class Stack>
	BinaryTree(const List<TreeNodeOrder<T>>& order, int model) {
		switch (model) {
		case 0x00:
		_preOrderIterativeBuildWithNode<List, Stack>(order);
		return;
		case 0x01:
		_preOrderRecursiveBuildWithNode<List, Stack>(order, 0, order.size());
		return;
		case 0x10:
		_levelOrderIterativeBuildWithNode<List, Stack>(order);
		return;
		}
	}
	//��ǰ�к����������������
	BinaryTree(BTree** orderA, BTree** orderB, size_t length, int model) {

	}
	//������
	BinaryTree(const BTree& other) :element(other.element) {
		if (other.lChild != nullptr) {
			lChild = new BTree(*(other.lChild));
		}
		if (other.rChild != nullptr) {
			rChild = new BTree(*(other.rChild));
		}
	}
	//�ƶ���	
	BinaryTree(BTree&& other) :lChild(other.lChild), rChild(other.rChild), element(std::move(other.element)) {
		other.lChild = nullptr;
		other.rChild = nullptr;
	}
	//��������ӽڵ�
	~BinaryTree() {
		delete lChild;
		delete rChild;
		lChild = nullptr;
		rChild = nullptr;
	}
	//������ֵ����
	BTree& operator=(const BTree& other) {
		if (this != &other) {
			delete lChild;
			delete rChild;
			if (other.lChild != nullptr)
				lChild = new BTree(*(other.lChild));
			else
				lChild = nullptr;
			if (other.rChild != nullptr)
				rChild = new BTree(*(other.rChild));
			else
				rChild = nullptr;
			element = other.element;
		}
		return *this;
	}
	//�ƶ���ֵ����
	BTree& operator=(BTree&& other)noexcept {
		if (this != &other) {
			delete lChild;
			delete rChild;
			lChild = other.lChild;
			rChild = other.rChild;
			element = std::move(other.element);
			other.lChild = nullptr;
			other.rChild = nullptr;
		}
		return *this;
	}
	inline static void deleteNode(const BTree*& node) {
		if (node) {
			node->lChild = nullptr;
			node->rChild = nullptr;
			delete node;
			node = nullptr;
		}
	}
	//ͨ������ǵݹ����������
	template <template<typename>class Stack>
	void _preClearIterative() const {
		Stack<BTree*> stack;
		BTree* ptr = this, * prev = nullptr;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				prev = ptr;
				stack.push(ptr->rChild); // ��������ջ
				ptr = ptr->lChild; // ������ڵ�
				if (prev != this) {//�����ýڵ�
					prev->lChild = nullptr;
					prev->rChild = nullptr;
					delete prev;
				}
			}
			else {//�սڵ�,����ջ
				ptr = stack.top();
				stack.pop();
			}
		}
		return nullptr;
	}
	//ͨ������ǵݹ����������
	template <template<typename>class Stack>
	void _inClearIterative() {
		Stack<BTree*> stack;
		BTree* ptr = this, * prev = nullptr;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				stack.push(ptr);
				ptr = ptr->lChild; // ������ڵ�
			}
			else {//�սڵ�,���˵�ջ���������ҽڵ�
				prev = stack.top();
				stack.pop();//�ýڵ�������ȫ������,��ջ
				ptr = prev->rChild;
				if (prev != this) {//�����ýڵ�
					prev->lChild = nullptr;
					prev->rChild = nullptr;
					delete prev;
				}
			}
		}
	}
	//ͨ������ǵݹ����������
	template <template<typename>class Stack>
	void _postClearIterative() {
		Stack<BTree*> stack;
		BTree* ptr = this, * prev = nullptr;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr && ptr != prev) {
				stack.push(ptr);
				ptr = ptr->lChild;//������ڵ�
			}
			else {//�սڵ�,���˵�ջ���������ҽڵ�
				prev = stack.top();
				if (ptr == prev->rChild) {//�ýڵ����������������,��Ȿ�ڵ�
					ptr = prev;
					if (prev != this) {
						prev->lChild = nullptr;
						prev->rChild = nullptr;
						delete prev;
					}
					stack.pop();//���ڵ������,��ջ
				}
				else
					ptr = ptr->rChild;
			}
		}
		return;
	}
	//���ر��ڵ�����
	inline T& getElement() { return element; }
	inline const T& getElement()const { return element; }
	//������ڵ�ָ��
	inline BTree* getLeftChildLink() { return lChild; }
	inline const BTree* getLeftChildLink()const { return lChild; }
	//�����ҽڵ�ָ��
	inline BTree* getRightChildLink() { return rChild; }
	inline const BTree* getRightChildLink()const { return rChild; }
	//������ڵ�����
	T& getLeftChildElement() {
		if (lChild != nullptr) {
			return lChild->element;
		}
#ifdef DEBUG
		throw std::out_of_range("Cannot find left child");
#endif // DEBUG
		return nullRef<T>();
	}
	const T& getLeftChildElement()const {
		if (lChild != nullptr) {
			return lChild->element;
		}
#ifdef DEBUG
		throw std::out_of_range("Cannot find left child");
#endif // DEBUG
		return T();
	}
	//�����ҽڵ�����
	T& getRightChildElement() {
		if (rChild != nullptr) {
			return rChild->element;
		}
#ifdef DEBUG
		throw std::out_of_range("Cannot find right child");
#endif // DEBUG
		return nullRef<T>();
	}
	const T& getRightChildElement()const {
		if (rChild != nullptr) {
			return rChild->element;
		}
#ifdef DEBUG
		throw std::out_of_range("Cannot find right child");
#endif // DEBUG
		return T();
	}
	//������ڵ������� ���ظ��ڵ�
	template <template<typename>class Stack>
	BTree* getParent(BTree* other) {
		if (other == this) {
#ifdef DEBUG
			throw std::out_of_range("Cannot find roots parent");
#endif // DEBUG
			return nullptr;
		}
		Stack<BTree*> stack;
		BTree* ptr = this, prev = nullptr;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr && ptr != prev) {
				stack.push(ptr);
				ptr = ptr->lChild;//������ڵ�
			}
			else {//�սڵ�,���˵�ջ���������ҽڵ�
				prev = stack.top();
				if (ptr == prev->rChild) {
					ptr = prev;
					stack.pop();
					if (ptr == other) {
						return stack.top();
					}
				}
				else
					ptr = ptr->rChild;
			}
		}
		return nullptr;
	}
	//�������ӽڵ㲢��ֵ Ĭ�������ӽڵ�
	BTree* _createLeftChild(const T& value) {
		lChild = new BTree(value);
		return lChild;
	}
	//�������ӽڵ㲢��ֵ
	BTree* _createRightChild(const T& value) {
		rChild = new BTree(value);
		return rChild;
	}
	//�Ƴ����ӽڵ㼰����
	void clearLeftChild() {
		delete lChild;
		lChild = nullptr;
	}
	//�Ƴ���ֵ�ڵ㼰����
	void clearRightChild() {
		delete rChild;
		rChild = nullptr;
	}
	//�������ӽڵ�
	void _connectLeftChild(BTree* left) {
		lChild = left;
	}
	void _connectLeftChild(BTree& left) {
		lChild = &left;
	}
	//�������ӽڵ�
	void _connectRightChild(BTree* right) {
		rChild = right;
	}
	void _connectRightChild(BTree& right) {
		rChild = &right;
	}
	//����ȫ�ӽڵ�
	void _connectChild(BTree* left, BTree* right) {
		lChild = left;
		rChild = right;
	}
	//�������ӽڵ�
	BTree* _disconnectLeftChild() {
		BTree* temp = lChild;
		lChild = nullptr;
		return temp;
	}
	//�������ӽڵ�
	BTree* _disconnectRightChild() {
		BTree* temp = rChild;
		rChild = nullptr;
		return temp;
	}
	//����ȫ�ӽڵ�
	void _disconnectChild() {
		lChild = nullptr;
		rChild = nullptr;
	}
	//����
	/*
			B
		  /   \
		T1		A
				| \
				T2	T3

			A
		  /   \
		B		T3
		| \
		T1	T2
	*/
	static void rotateLeft(BTree*& root) {
		if (root == nullptr || root->rChild == nullptr) {
			return;
		}
		else {
			BTree* newRoot = root->rChild;
			root->rChild = newRoot->lChild;
			newRoot->lChild = root;
			root = newRoot;
			return;
		}
	}
	//����
	/*
			A
		  /   \
		B		T3
		| \
		T1	T2

			B
		  /   \
		T1		A
				| \
				T2	T3
	*/
	static void rotateRight(BTree*& root) {
		if (root == nullptr || root->lChild == nullptr) {
			return;
		}
		else {
			BTree* newRoot = root->lChild;
			root->lChild = newRoot->rChild;
			newRoot->rChild = root;
			root = newRoot;
			return;
		}
	}
	//������Ƿ���ѭ����
	bool detectTreeCycle() {

	}
	//�������
	size_t depthRecursive()const {
		size_t lDepth = 0;
		size_t rDepth = 0;
		if (lChild != nullptr)
			lDepth = lChild->depthRecursive();
		if (rChild != nullptr)
			rDepth = rChild->depthRecursive();
		return lDepth > rDepth ? lDepth + 1 : rDepth + 1;
	}
	template <template<typename>class Stack>
	size_t depthIterative()const {
		size_t maxDepth = 0;
		Stack<BTree*> stack;
		BTree* ptr = const_cast<BTree*>(this);
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				stack.push(ptr->rChild); // ��������ջ
				maxDepth = maxDepth > stack.size() ? maxDepth : stack.size();//���³���
				ptr = ptr->lChild;//������ڵ�
			}
			else {//�սڵ�,���˵�ջ���������ҽڵ�
				ptr = stack.top();
				stack.pop();//�ýڵ�������ȫ������,��ջ
			}
		}
		return maxDepth;
	}
	//Ԫ������
	size_t sizeRecursive() const {
		size_t size = 1;
		Function fun = Function(
			[&size](BTree* node) {
				++size;
			});
		_preOrderRecursiveWithFunctionTraverse(fun);
		return size;
	}
	template <template<typename>class Stack>
	size_t sizeIterative() const {
		size_t size = 1;
		Function fun = Function(
			[&size](BTree* node) {
				++size;
			});
		_preOrderIterativeWithFunctionTraverse<Stack>(fun);
		return size;
	}
	//�Գ���
	void symmetryRecursive() {
		auto fun = Function(
			[](BTree* node) {
				BTree* left = node->lChild;
				BTree* right = node->rChild;
				node->rChild = left;
				node->lChild = right;
			});
		_preOrderRecursiveWithFunctionTraverse(fun);
		return;
	}
	template <template<typename>class Stack>
	void symmetryIterative() {
		auto fun = Function(
			[](BTree* node) {
				BTree* left = node->lChild;
				BTree* right = node->rChild;
				node->rChild = left;
				node->lChild = right;
			});
		_preOrderIterativeWithFunctionTraverse<Stack>(fun);
		return;
	}
	//��3��ģʽ���������Ƿ���valueԪ�� �������ڽڵ�
	/*
	00Ϊ�������  �ڵ� �������� ��������
	01Ϊ�������  �������� �ڵ� ��������
	10Ϊ�������  �������� �������� �ڵ�
	11Ϊ�������
	0xxΪ�ݹ����
	1xxΪ�ǵݹ����
	*/
	template <template<typename>class Stack>
	BTree* find(const T& value, int model = 0) {
		Condition conditionFun = Condition(
			[&value](BTree* node)->bool {return node->getElement() == value; });
		return orderCondition<Stack>(conditionFun, model);
	}
	//����condition�жϣ����Ϊtrue���ؽڵ�
	template <typename Condition, template<typename>class Stack>
	BTree* orderCondition(Condition& cond, int model = 0) {
		switch (model) {
		case 0b000:
		return _preOrderRecursiveWithCondition(cond);
		case 0b001:
		return _inOrderRecursiveWithCondition(cond);
		case 0b010:
		return _postOrderRecursiveWithCondition(cond);
		case 0b100:
		return _preOrderIterativeWithCondition<Stack>(cond);
		case 0b101:
		return _inOrderIterativeWithCondition<Stack>(cond);
		case 0b110:
		return _postOrderIterativeWithCondition<Stack>(cond);
		}
#ifdef DEBUG
		throw std::out_of_range("Not within options");
#endif // DEBUG
		return nullptr;
	}
	/*
	pre����ǰ�����  in�����������  post����������	level����������
	Recursive����ݹ����  Iterative����ǵݹ����
	Print��ʾ���������  PrintWithNode�����пսڵ��æ�����
	FunctionTraverse���������Ԫ��ִ��Function
	Condition������ҷ��ص�һ��ʹConditionΪTRUE�Ķ���
	ConditionFunction���������ConditionΪTRUE�Ķ���ִ��Function
	����PrintWithNode�������Ϊ ÿһ���ڵ��һ���� �޷���������й����� ���������Ҫ��ת������� ����ǰ������� �ٶԳ���
	*/
	//�������� List Ϊ��
	template <template<typename U>class List>
	List<TreeNodeOrder<T>> _preOrderRecursivePrintWithNode() {
		List<TreeNodeOrder<T>> list;
		list.push_back(TreeNodeOrder<T>(element, false));
		if (lChild != nullptr)
			list.linkDoubleLists(lChild->_preOrderRecursivePrintWithNode());
		else
			list.push_back(TreeNodeOrder<T>());
		if (rChild != nullptr)
			list.linkDoubleLists(rChild->_preOrderRecursivePrintWithNode());
		else
			list.push_back(TreeNodeOrder<T>());
		return list;
	}
	template <template<typename U>class List>
	List<T> _preOrderRecursivePrint() {
		List<T> list;
		list.push_back(element);
		if (lChild != nullptr)
			list.linkDoubleLists(lChild->_preOrderRecursivePrint());
		if (rChild != nullptr)
			list.linkDoubleLists(rChild->_preOrderRecursivePrint());
		return list;
	}
	template <template<typename U>class List>
	List<T> _inOrderRecursivePrint() {
		List<T> list;
		if (lChild != nullptr)
			list.linkDoubleLists(lChild->_inOrderRecursivePrint());
		list.push_back(element);
		if (rChild != nullptr)
			list.linkDoubleLists(rChild->_inOrderRecursivePrint());
		return list;
	}
	template <template<typename U>class List>
	List<T> _postOrderRecursivePrint() {
		List<T> list;
		if (lChild != nullptr)
			list.linkDoubleLists(lChild->_postOrderRecursivePrint());
		if (rChild != nullptr)
			list.linkDoubleLists(rChild->_postOrderRecursivePrint());
		list.push_back(element);
		return list;
	}
	/*
				a
			b		c
		d	e			f
	   g h i
	ptr=a
	stack=c	fun(a)	ptr=b
	stack=c,e	fun(b)	ptr=d
	stack=c,e,h	fun(d)	ptr=g
	stack=c,e,h,N	fun(g)	ptr=NULL
	fun(��)	ptr=NULL	stack=c,e,h
	fun(��)	ptr=h	stack=c,e
	stack=c,e,N	fun(h)	ptr=NULL
	fun(��)	ptr=NULL	stack=c,e
	fun(��)	ptr=e	stack=c
	stack=c,N	fun(e)	ptr=i
	stack=c,N,N	fun(i)	ptr=NULL
	fun(��)	ptr=N	stack=c,N
	fun(��)	ptr=N	stack=c
	fun(��)	ptr=c	stack=
	stack=f	fun(c)	ptr=NULL
	fun(��)	ptr=f	stack=
	stack=N	fun(f)	ptr=NULL
	fun(��)	ptr=N	stack=
	end
	fun(��)
	a,b,d,g,��,��,h,��,��,e,i,��,��,��,c,��,f,��,��
	*/
	template <template<typename U>class List, template<typename>class Stack>
	List<TreeNodeOrder<T>> _preOrderIterativePrintWithNode() {
		List<TreeNodeOrder<T>> list;
		Stack<BTree*> stack;
		BTree* ptr = this;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				list.push_back(TreeNodeOrder<T>(ptr->element, false)); // �����ѷ��ʵĽڵ�
				stack.push(ptr->rChild); // ��������ջ
				ptr = ptr->lChild; // ������ڵ�
			}
			else {
				list.push_back(TreeNodeOrder<T>()); // �սڵ�
				ptr = stack.top();
				stack.pop();
			}
		}
		list.push_back(TreeNodeOrder<T>());//���Ŀսڵ�
		return list;
	}
	template <template<typename U>class List, template<typename>class Stack>
	List<T> _preOrderIterativePrint() {
		List<T> list;
		Stack<BTree*> stack;
		BTree* ptr = this;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				list.push_back(ptr->element);//���뱾�ڵ�
				stack.push(ptr->rChild); // ��������ջ
				ptr = ptr->lChild; // ������ڵ�
			}
			else {//�սڵ�,����ջ
				ptr = stack.top();
				stack.pop();
			}
		}
		return list;
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
	stack=a,b,d,g	ptr=NULL
	ptr=g	fun(g)	stack=a,b,d	ptr=NULL
	ptr=d	fun(d)	stack=a,b	ptr=h
	stack=a,b,h	ptr=NULL
	ptr=h	fun(h)	stack=a,b	ptr=NULL
	ptr=b	fun(b)	stack=a	ptr=e
	stack=a,e	ptr=i
	stack=a,e,i	ptr=NULL
	ptr=i	fun(i)	stack=a,e	ptr=NULL
	ptr=e	fun(e)	stack=a	ptr=NULL
	ptr=a	fun(a)	stack=	ptr=c
	stack=c	ptr=NULL
	ptr=c	fun(c)	stack=	ptr=f
	stack=f ptr=NULL
	ptr=f	fun(f)	stack=	ptr=NULL
	end
	g,d,h,b,i,e,a,c,f
	*/
	template <template<typename U>class List, template<typename>class Stack>
	List<T> _inOrderIterativePrint() {
		List<T> list;
		Stack<BTree*> stack;
		BTree* ptr = this;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				stack.push(ptr);
				ptr = ptr->lChild;//������ڵ�
			}
			else {//�սڵ�,���˵�ջ���������ҽڵ�
				ptr = stack.top();
				list.push_back(ptr->element);
				stack.pop();//�ýڵ�������ȫ������,��ջ
				ptr = ptr->rChild;
			}
		}
		return list;
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
	pv=back()=g	ptr==pv->r	ptr=pop()=g	fun(g)	stack=a,b,d
	pv=back()=d	ptr!=pv->r	ptr=h
	stack=a,b,d,h	ptr=Null
	pv=back()=h	ptr==pv->r	ptr=pop()=h	fun(h)	stack=a,b,d
	pv=back()=d	ptr==pv->r	ptr=pop()=d	fun(d)	stack=a,b
	pv=back()=b	ptr!=pv->r	ptr=e
	stack=a,b,e	ptr=i
	stack=a,b,e,i	ptr=Null
	pv=back()=i	ptr==pv->r	ptr=pop()=i	fun(i)	stack=a,b,e
	pv=back()=e	ptr!=pv->r	ptr=Null
	pv=back()=e	ptr==pv->r	ptr=pop()=e	fun(e)	stack=a,b
	pv=back()=b	ptr!=pv->r	ptr=Null
	pv=back()=b	ptr==pv->r	ptr=pop()=b	fun(b)	stack=a
	pv=back()=a	ptr==pv->r	ptr=c
	stack=a,c	ptr=Null
	pv=back()=c	ptr!=pv->r	ptr=f
	stack=a,c,f	ptr=Null
	pv=back()=f	ptr==pv->r	ptr=pop()=f	fun(f)	stack=a,c
	pv=back()=c	ptr==pv->r	ptr=pop()=c	fun(c)	stack=a
	pv=back()=a	ptr==pv->r	ptr=pop()=a	fun(a)	stack=
	end
	g,h,d,i,e,b,f,c,a
	*/
	template <template<typename U>class List, template<typename>class Stack>
	List<T> _postOrderIterativePrint() {
		List<T> list;
		Stack<BTree*> stack;
		BTree* ptr = this, * prev = nullptr;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr && ptr != prev) {
				stack.push(ptr);
				ptr = ptr->lChild;//������ڵ�
			}
			else {//�սڵ�,���˵�ջ���������ҽڵ�
				prev = stack.top();
				if (ptr == prev->rChild) {//�ýڵ����������������,��Ȿ�ڵ�
					ptr = prev;
					list.push_back(ptr->element);
					stack.pop();//���ڵ������,��ջ
				}
				else
					ptr = ptr->rChild;
			}
		}
		return list;
	}
	/*
				a
			b		c
		d	e			f
	   g h i
	ptr=this	queue=a;
	ptr=a	fun(a)	queue=b,c
	ptr=b	fun(b)	queue=c,d,e
	ptr=c	fun(c)	queue=d,e,��,f
	ptr=d	fun(d)	queue=e,��,f,g,h
	ptr=e	fun(e)	queue=��,f,g,h,i,��
	ptr=��	queue=f,g,h,i,��	ptr=f	fun(f)	queue=g,h,i,��,��,��
	ptr=g	queue=h,i,��,��,��,��,��
	ptr=h	queue=i,��,��,��,��,��,��,��
	ptr=i	queue=��,��,��,��,��,��,��,��,��
	ptr=��	queue=��,��,��,��,��,��,��,��	ptr=��	queue=��,��,��,��,��,��,��	ptr=��	queue=��,��,��,��,��,��
	ptr=��	queue=��,��,��,��,��	ptr=��	queue=��,��,��,��	ptr=��	queue=��,��,��	ptr=��	queue=��,��	ptr=��	queue=��
	ptr=��	queue=	end
	a,b,c,d,e,��,f,g,h,i,��,��,��,��,��,��,��,��,��
	*/
	template <template<typename U>class List, template<typename>class Queue>
	List<TreeNodeOrder<T>> _levelOrderIterativePrintWithNode() {
		List<TreeNodeOrder<T>> list;
		Queue<T> queue;
		BTree* ptr = this;
		queue.push(ptr);
		while (!queue.empty()) {
			ptr = queue.back();
			queue.pop();
			if (ptr == nullptr) {
				list.push_back(TreeNodeOrder<T>());
			}
			else {
				list.push_back(ptr->element, false);
				queue.push(ptr->lChild);
				queue.push(ptr->rChild);
			}
		}
		return list;
	}
	template <template<typename U>class List, template<typename>class Queue>
	List<T> _levelOrderIterativePrint() {
		List<T> list;
		Queue<BTree*> queue;
		BTree* ptr = this;
		queue.push(ptr);
		while (!queue.empty()) {
			ptr = queue.back();
			queue.pop();
			if (ptr != nullptr) {
				list.push_back(ptr->element);
				queue.push(ptr->lChild);
				queue.push(ptr->rChild);
			}
		}
		return list;
	}
	//ʹ�ýڵ����й�����
	/*
				a
			b		c
		d	e			f
	   g h i
	A,B,D,G,��,��,H,��,��,E,I,��,��,��,C,��,F,��,�� this���T	stack���S	ptr���P	prev���V
	A S=A V=A P=AnewL 	B S=A,B V=B P=BnewL 	D S=A,B,D V=D P=DnewL	G S=A,B,D,G V=G P=GnewL
	�� (P=V->L) DetP V=G P=V->R=GnewR S=A,B,D
	�� (P=V->R) DetP P=G V=Nul (V=P->R) {V=G P=D S=A,B} V=D P=DnewR
	H S=A,B,H V=H P=HnewL
	�� (P=V->L) DetP V=H P=V->R=HnewR S=A,B
	�� (P=V->R) DetP P=H V=Nul (V=P->R) {V=H P=B S=A} V=B P=BnewR
	E S=A,E V=E P=EnewL		I S=A,E,I V=I P=InewL
	�� (P=V->L) DetP V=I P=V->R=InewR S=A,E
	�� (P=V->R) DetP P=I V=Nul (V=P->R) {V=I P=E S=A} V=E P=EnewR
	�� (P=V->R) DetP P=E V=Nul (V=P->R) {V=E P=A S=} V=A P=AnewR
	C S=C V=C P=CnewL
	�� (P=V->L) DetP V=C P=V->R=CnewR S=
	F S=F V=F P=FnewL
	�� (P=V->L) DetP V=F P=V->R=FnewR S=
	�� (P=V->R) DetP P=F P=Null (V=P->R) {return}
	*/
	template <template<typename U>class List, template<typename>class Stack>
	void _preOrderIterativeBuildWithNode(const List<TreeNodeOrder<T>>& order) {
		Stack<BTree*> stack;
		BTree* ptr = this, * prev = nullptr;
		for (size_t i = 0; i < order.size(); ++i) {
			if (!order[i].isEmpty) {//P=V->L��P=V->R��P=V->L״̬ת��
				ptr->element = order[i].element;
				stack.push(ptr);
				ptr->lChild = new BTree();
				ptr->rChild = new BTree();
				prev = ptr;
				ptr = ptr->lChild;
			}
			else {
				if (prev->lChild == ptr) {//�����������󸳿�,��������ת��P=V->L��P=V->R״̬ת��
					delete prev->lChild;
					prev->lChild = nullptr;
					ptr = prev->rChild;
					stack.pop();
				}
				else {//�����������󸳿�,��ʼ����P=V->R��V=P->R״̬ת��
					delete prev->rChild;
					prev->rChild = nullptr;
					ptr = prev;
					prev = ptr->rChild;
					while (ptr->rChild == prev) {//�������ڵ�Ϊ������,V=P->R��V=P->R,V=P->Lת��
						if (stack.empty())
							return;
						prev = ptr;
						ptr = stack.top();
						stack.pop();
					}
					prev = ptr;//���������������������V=P->L��P=V->Rת��
					ptr = prev->rChild;
				}
			}
		}
		return;
	}
	template <template<typename U>class List, template<typename>class Stack>
	void _preOrderRecursiveBuildWithNode(const List<TreeNodeOrder<T>>& order, size_t begin, size_t end) {
		element = order[begin];
		size_t i = begin + 1;
		for (int count = 0; i != end || count == 1; ++i) {//[����,�ָ��]:�ڵ�=�սڵ�+1
			if (order[i].isEmpty())
				++count;
			else
				--count;
		}
		if (order[begin + 1].isEmpty()) {
			lChild = nullptr;
		}
		else {
			lChild->_preOrderRecursiveBuildWithNode(order, begin + 1, i);
		}
		if (order[i].isEmpty()) {
			rChild = nullptr;
		}
		else {
			rChild->_preOrderRecursiveBuildWithNode(order, i, end);
		}
		return;
	}
	template <template<typename U>class List, template<typename>class Queue>
	void _levelOrderIterativeBuildWithNode(const List<TreeNodeOrder<T>>& order) {
		struct Node {
			BTree* ptr;
			bool isleft;
			Node(BTree* inPtr = nullptr, bool inIsLeft = 0) :ptr(inPtr), isleft(inIsLeft) {};
		};
		Queue<Node> queue;
		BTree* tempptr = this;
		bool tempisleft;
		tempptr = order[0].element;
		queue.push(Node(tempptr, 1));
		queue.push(Node(tempptr, 0));
		for (size_t i = 1; i < order.size(); ++i) {
			tempptr = queue.back().ptr;
			tempisleft = queue.back().isleft;
			if (tempisleft == 1) {
				if (!order[i].isEmpty()) {
					tempptr->lChild = new BTree(List[i].element);
					queue.push(Node(tempptr, 1));
					queue.push(Node(tempptr, 0));
				}
				else {
					tempptr->lChild = nullptr;
				}
			}
			else {
				if (!order[i].isEmpty()) {
					tempptr->rChild = new BTree(List[i].element);
					queue.push(Node(tempptr, 1));
					queue.push(Node(tempptr, 0));
				}
				else {
					tempptr->rChild = nullptr;
				}
			}
		}
		return;
	}
	//�����нڵ����Function����
	template <typename Function>
	void _preOrderRecursiveWithFunctionTraverse(Function&& func) {
		func(this);
		if (lChild != nullptr)
			lChild->_preOrderRecursiveWithFunctionTraverse(func);
		if (rChild != nullptr)
			rChild->_preOrderRecursiveWithFunctionTraverse(func);
		return;
	}
	template <typename Function>
	void _inOrderRecursiveWithFunctionTraverse(Function&& func) {
		if (lChild != nullptr)
			lChild->_inOrderRecursiveWithFunctionTraverse(func);
		func(this);
		if (rChild != nullptr)
			rChild->_inOrderRecursiveWithFunctionTraverse(func);
		return;
	}
	template <typename Function>
	void _postOrderRecursiveWithFunctionTraverse(Function&& func) {
		if (lChild != nullptr)
			lChild->_postOrderRecursiveWithFunctionTraverse(func);
		if (rChild != nullptr)
			rChild->_postOrderRecursiveWithFunctionTraverse(func);
		func(this);
		return;
	}
	template <template<typename>class Stack, typename Function>
	void _preOrderIterativeWithFunctionTraverse(Function&& func) {
		Stack<BTree*> stack;
		BTree* ptr = this;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				func(ptr);//���뱾�ڵ�
				stack.push(ptr->rChild); // ��������ջ
				ptr = ptr->lChild; // ������ڵ�
			}
			else {//�սڵ�,����ջ
				ptr = stack.top();
				stack.pop();
			}
		}
		return;
	}
	template <template<typename>class Stack, typename Function>
	void _inOrderIterativeWithFunctionTraverse(Function&& func) {
		Stack<BTree*> stack;
		BTree* ptr = this;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				stack.push(ptr);
				ptr = ptr->lChild;//������ڵ�
			}
			else {//�սڵ�,���˵�ջ���������ҽڵ�
				ptr = stack.top();
				func(ptr);//�����������ڵ�
				stack.pop();//�ýڵ�������ȫ������,��ջ
				ptr = ptr->rChild;
			}
		}
		return;
	}
	template <template<typename>class Stack, typename Function>
	void _postOrderIterativeWithFunctionTraverse(Function&& func) {
		Stack<BTree*> stack;
		BTree* ptr = this, * prev = nullptr;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr && ptr != prev) {
				stack.push(ptr);
				ptr = ptr->lChild;//������ڵ�
			}
			else {//�սڵ�,���˵�ջ���������ҽڵ�
				if (stack.empty())
					return;
				prev = stack.top();
				if (ptr == prev->rChild) {//�ýڵ����������������,��Ȿ�ڵ�
					ptr = prev;
					func(ptr);
					stack.pop();//���ڵ������,��ջ
				}
				else
					ptr = prev->rChild;
			}
		}
		return;
	}
	template <template<typename>class Queue, typename Function>
	void _levelOrderIterativeWithFunctionTraverse(Function&& func) {
		Queue<BTree*> queue;
		BTree* ptr = this;
		queue.push(ptr);
		while (!queue.empty()) {
			ptr = queue.back();
			queue.pop();
			if (ptr != nullptr) {
				func(ptr);
				queue.push(ptr->lChild);
				queue.push(ptr->rChild);
			}
		}
		return;
	}
	//���ҷ���Conditionν�ʵĵ�һ������
	template <typename Condition>
	BTree* _preOrderRecursiveWithCondition(Condition&& cond) {
		BTree* that = nullptr;
		if (cond(this) && that == nullptr)
			that = this;//����ֱ��return this ��������������
		if (lChild != nullptr && that == nullptr)
			that = lChild->_preOrderRecursiveWithCondition(cond);
		if (rChild != nullptr && that == nullptr)
			that = rChild->_preOrderRecursiveWithCondition(cond);
		return that;
	}
	template <typename Condition>
	BTree* _inOrderRecursiveWithCondition(Condition&& cond) {
		BTree* that = nullptr;
		if (lChild != nullptr && that == nullptr)
			that = lChild->_inOrderRecursiveWithCondition(cond);
		if (cond(this) && that == nullptr)
			that = this;
		if (rChild != nullptr && that == nullptr)
			that = rChild->_inOrderRecursiveWithCondition(cond);
		return that;
	}
	template <typename Condition>
	BTree* _postOrderRecursiveWithCondition(Condition&& cond) {
		BTree* that = nullptr;
		if (lChild != nullptr && that == nullptr)
			that = lChild->_postOrderRecursiveWithCondition(cond);
		if (rChild != nullptr && that == nullptr)
			that = rChild->_postOrderRecursiveWithCondition(cond);
		if (cond(this) && that == nullptr)
			that = this;
		return that;
	}
	template <template<typename>class Stack, typename Condition>
	BTree* _preOrderIterativeWithCondition(Condition&& cond) {
		Stack<BTree*> stack;
		BTree* ptr = this;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				if (cond(ptr))//�����������ڵ�
					return ptr;
				stack.push(ptr->rChild); // ��������ջ
				ptr = ptr->lChild; // ������ڵ�
			}
			else {//�սڵ�,����ջ
				ptr = stack.top();
				stack.pop();
			}
		}
		return nullptr;
	}
	template <template<typename>class Stack, typename Condition>
	BTree* _inOrderIterativeWithCondition(Condition&& cond) {
		Stack<BTree*> stack;
		BTree* ptr = this;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				stack.push(ptr);
				ptr = ptr->lChild;//������ڵ�
			}
			else {//�սڵ�,���˵�ջ���������ҽڵ�
				ptr = stack.top();
				if (cond(ptr))//�����������ڵ�
					return ptr;
				stack.pop();//�ýڵ�������ȫ������,��ջ
				ptr = ptr->rChild;
			}
		}
		return nullptr;
	}
	template <template<typename>class Stack, typename Condition>
	BTree* _postOrderIterativeWithCondition(Condition&& cond) {
		Stack<BTree*> stack;
		BTree* ptr = this, * prev = nullptr;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr && ptr != prev) {
				stack.push(ptr);
				ptr = ptr->lChild;//������ڵ�
			}
			else {//�սڵ�,���˵�ջ���������ҽڵ�
				prev = stack.top();
				if (ptr == prev->rChild) {//�ýڵ����������������,��Ȿ�ڵ�
					ptr = prev;
					if (cond(ptr)) {
						return ptr;
					}
					stack.pop();//���ڵ������,��ջ
				}
				else
					ptr = ptr->rChild;
			}
		}
		return nullptr;
	}
	template <template<typename>class Queue, typename Condition>
	BTree* _levelOrderIterativeWithCondition(Condition&& cond) {
		Queue<BTree*> queue;
		BTree* ptr = this;
		queue.push(ptr);
		while (!queue.empty()) {
			ptr = queue.back();
			queue.pop();
			if (ptr != nullptr) {
				if (cond(ptr)) {
					return ptr;
				}
				queue.push(ptr->lChild);
				queue.push(ptr->rChild);
			}
		}
		return nullptr;
	}
	//�Խڵ����condition�жϣ����Ϊtrue ��Ԫ��ִ��function
	template <typename Condition, typename Function>
	void _preOrderRecursiveWithConditionFunction(Condition&& cond, Function&& func) {
		if (cond(this))
			func(this);
		if (lChild != nullptr)
			lChild->_preOrderRecursiveWithConditionFunction(cond, func);
		if (rChild != nullptr)
			rChild->_preOrderRecursiveWithConditionFunction(cond, func);
		return;
	}
	template <typename Condition, typename Function>
	void _inOrderRecursiveWithConditionFunction(Condition&& cond, Function& func) {
		if (lChild != nullptr)
			lChild->_inOrderRecursiveWithConditionFunction(cond, func);
		if (cond(this))
			func(this);
		if (rChild != nullptr)
			rChild->_inOrderRecursiveWithConditionFunction(cond, func);
		return;
	}
	template <typename Condition, typename Function>
	void _postOrderRecursiveWithConditionFunction(Condition&& cond, Function&& func) {
		if (lChild != nullptr)
			lChild->_postOrderRecursiveWithConditionFunction(cond, func);
		if (rChild != nullptr)
			rChild->_postOrderRecursiveWithConditionFunction(cond, func);
		if (cond(this))
			func(this);
		return;
	}
	template <template<typename>class Stack, typename Condition, typename Function>
	void _preOrderIterativeWithConditionFunction(Condition&& cond, Function&& func) {
		Stack<BTree*> stack;
		BTree* ptr = this;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				if (cond(ptr))//�����������ڵ�
					func(ptr);
				stack.push(ptr->rChild);
				ptr = ptr->lChild;//������ڵ�
			}
			else {//�սڵ�,���˵�ջ���������ҽڵ�
				ptr = stack.top();
				stack.pop();//�ýڵ�������ȫ������,��ջ
			}
		}
		return;
	}
	template <template<typename>class Stack, typename Condition, typename Function>
	void _inOrderIterativeWithConditionFunction(Condition&& cond, Function&& func) {
		Stack<BTree*> stack;
		BTree* ptr = this;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				stack.push(ptr);
				ptr = ptr->lChild;//������ڵ�
			}
			else {//�սڵ�,���˵�ջ���������ҽڵ�
				ptr = stack.top();
				if (cond(ptr))//�����������ڵ�
					func(ptr);
				stack.pop();//�ýڵ�������ȫ������,��ջ
				ptr = ptr->rChild;
			}
		}
		return;
	}
	template <template<typename>class Stack, typename Condition, typename Function>
	void _postOrderIterativeWithConditionFunction(Condition&& cond, Function&& func) {
		Stack<BTree*> stack;
		BTree* ptr = this, * prev = nullptr;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr && ptr != prev) {
				stack.push(ptr);
				ptr = ptr->lChild;//������ڵ�
			}
			else {//�սڵ�,���˵�ջ���������ҽڵ�
				prev = stack.top();
				if (ptr == prev->rChild) {//�ýڵ����������������,��Ȿ�ڵ�
					ptr = prev;
					if (cond(ptr))
						func(ptr);
					stack.pop();//���ڵ������,��ջ
				}
				else
					ptr = ptr->rChild;
			}
		}
		return;
	}
	template <template<typename>class Queue, typename Condition, typename Function>
	void _levelOrderIterativeWithConditionFunction(Condition&& cond, Function&& func) {
		Queue<BTree*> queue;
		BTree* ptr;
		queue.push(this);
		while (!queue.empty()) {
			ptr = queue.back();
			queue.pop();
			if (ptr != nullptr) {
				if (cond(ptr)) {
					func(ptr);
				}
				queue.push(ptr->lChild);
				queue.push(ptr->rChild);
			}
		}
		return;
	}
};
