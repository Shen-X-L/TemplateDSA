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
	T element;//内容元素
	BTree* lChild = nullptr;//左子节点 简称左节点
	BTree* rChild = nullptr;//右子节点 简称右节点
public:
	//无参构造函数
	BinaryTree() {}
	//创建叶子节点
	BinaryTree(const T& in) :element(in) {}
	//仅子节点内容
	BinaryTree(const T& left, const T& right)
		:lChild(new BTree(left)), rChild(new BTree(right)) {}
	//本节点与子节点内容
	BinaryTree(const T& in, const T& left, const T& right)
		:lChild(new BTree(left)), rChild(new BTree(right)), element(in) {}
	//子节点链接
	BinaryTree(const T& in, BTree* inLeft, BTree* inRight)
		:element(in), lChild(inLeft), rChild(inRight) {}
	//以某种遍历形式构建完全二叉树
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
	//靠前中后序遍历构建二叉树
	BinaryTree(BTree** orderA, BTree** orderB, size_t length, int model) {

	}
	//拷贝树
	BinaryTree(const BTree& other) :element(other.element) {
		if (other.lChild != nullptr) {
			lChild = new BTree(*(other.lChild));
		}
		if (other.rChild != nullptr) {
			rChild = new BTree(*(other.rChild));
		}
	}
	//移动树	
	BinaryTree(BTree&& other) :lChild(other.lChild), rChild(other.rChild), element(std::move(other.element)) {
		other.lChild = nullptr;
		other.rChild = nullptr;
	}
	//清空所有子节点
	~BinaryTree() {
		delete lChild;
		delete rChild;
		lChild = nullptr;
		rChild = nullptr;
	}
	//拷贝赋值函数
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
	//移动赋值函数
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
	//通过先序非递归遍历清理树
	template <template<typename>class Stack>
	void _preClearIterative() const {
		Stack<BTree*> stack;
		BTree* ptr = this, * prev = nullptr;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				prev = ptr;
				stack.push(ptr->rChild); // 右子树入栈
				ptr = ptr->lChild; // 访问左节点
				if (prev != this) {//断连该节点
					prev->lChild = nullptr;
					prev->rChild = nullptr;
					delete prev;
				}
			}
			else {//空节点,回退栈
				ptr = stack.top();
				stack.pop();
			}
		}
		return nullptr;
	}
	//通过中序非递归遍历清理树
	template <template<typename>class Stack>
	void _inClearIterative() {
		Stack<BTree*> stack;
		BTree* ptr = this, * prev = nullptr;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				stack.push(ptr);
				ptr = ptr->lChild; // 访问左节点
			}
			else {//空节点,回退到栈顶访问其右节点
				prev = stack.top();
				stack.pop();//该节点子树将全部遍历,退栈
				ptr = prev->rChild;
				if (prev != this) {//断连该节点
					prev->lChild = nullptr;
					prev->rChild = nullptr;
					delete prev;
				}
			}
		}
	}
	//通过后序非递归遍历清理树
	template <template<typename>class Stack>
	void _postClearIterative() {
		Stack<BTree*> stack;
		BTree* ptr = this, * prev = nullptr;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr && ptr != prev) {
				stack.push(ptr);
				ptr = ptr->lChild;//访问左节点
			}
			else {//空节点,回退到栈顶访问其右节点
				prev = stack.top();
				if (ptr == prev->rChild) {//该节点左右子树搜索完毕,检测本节点
					ptr = prev;
					if (prev != this) {
						prev->lChild = nullptr;
						prev->rChild = nullptr;
						delete prev;
					}
					stack.pop();//本节点检查完毕,退栈
				}
				else
					ptr = ptr->rChild;
			}
		}
		return;
	}
	//返回本节点内容
	inline T& getElement() { return element; }
	inline const T& getElement()const { return element; }
	//返回左节点指针
	inline BTree* getLeftChildLink() { return lChild; }
	inline const BTree* getLeftChildLink()const { return lChild; }
	//返回右节点指针
	inline BTree* getRightChildLink() { return rChild; }
	inline const BTree* getRightChildLink()const { return rChild; }
	//返回左节点内容
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
	//返回右节点内容
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
	//若输入节点在树内 返回父节点
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
				ptr = ptr->lChild;//访问左节点
			}
			else {//空节点,回退到栈顶访问其右节点
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
	//生成左子节点并赋值 默认无左子节点
	BTree* _createLeftChild(const T& value) {
		lChild = new BTree(value);
		return lChild;
	}
	//生成右子节点并赋值
	BTree* _createRightChild(const T& value) {
		rChild = new BTree(value);
		return rChild;
	}
	//移除左子节点及其树
	void clearLeftChild() {
		delete lChild;
		lChild = nullptr;
	}
	//移除右值节点及其树
	void clearRightChild() {
		delete rChild;
		rChild = nullptr;
	}
	//连接左子节点
	void _connectLeftChild(BTree* left) {
		lChild = left;
	}
	void _connectLeftChild(BTree& left) {
		lChild = &left;
	}
	//连接右子节点
	void _connectRightChild(BTree* right) {
		rChild = right;
	}
	void _connectRightChild(BTree& right) {
		rChild = &right;
	}
	//连接全子节点
	void _connectChild(BTree* left, BTree* right) {
		lChild = left;
		rChild = right;
	}
	//断连左子节点
	BTree* _disconnectLeftChild() {
		BTree* temp = lChild;
		lChild = nullptr;
		return temp;
	}
	//断连右子节点
	BTree* _disconnectRightChild() {
		BTree* temp = rChild;
		rChild = nullptr;
		return temp;
	}
	//断连全子节点
	void _disconnectChild() {
		lChild = nullptr;
		rChild = nullptr;
	}
	//左旋
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
	//右旋
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
	//检测树是否有循环节
	bool detectTreeCycle() {

	}
	//返回深度
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
				stack.push(ptr->rChild); // 右子树入栈
				maxDepth = maxDepth > stack.size() ? maxDepth : stack.size();//更新长度
				ptr = ptr->lChild;//访问左节点
			}
			else {//空节点,回退到栈顶访问其右节点
				ptr = stack.top();
				stack.pop();//该节点子树将全部遍历,退栈
			}
		}
		return maxDepth;
	}
	//元素数量
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
	//对称树
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
	//以3种模式遍历树内是否有value元素 返回所在节点
	/*
	00为先序遍历  节点 先序左树 先序右树
	01为中序遍历  中序左树 节点 中序右树
	10为后序遍历  后序左树 后序右树 节点
	11为层序遍历
	0xx为递归遍历
	1xx为非递归遍历
	*/
	template <template<typename>class Stack>
	BTree* find(const T& value, int model = 0) {
		Condition conditionFun = Condition(
			[&value](BTree* node)->bool {return node->getElement() == value; });
		return orderCondition<Stack>(conditionFun, model);
	}
	//进行condition判断，如果为true返回节点
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
	pre代表前序遍历  in代表中序遍历  post代表后序遍历	level代表层序遍历
	Recursive代表递归操作  Iterative代表非递归操作
	Print表示输出到表中  PrintWithNode中其中空节点用Φ代替
	FunctionTraverse代表对所有元素执行Function
	Condition代表查找返回第一个使Condition为TRUE的对象
	ConditionFunction代表对所有Condition为TRUE的对象执行Function
	其中PrintWithNode中序输出为 每一个节点隔一个Φ 无法单输出序列构建树 后序输出需要反转输出序列 按照前序规则构造 再对称树
	*/
	//创建序列 List 为表
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
	fun(Φ)	ptr=NULL	stack=c,e,h
	fun(Φ)	ptr=h	stack=c,e
	stack=c,e,N	fun(h)	ptr=NULL
	fun(Φ)	ptr=NULL	stack=c,e
	fun(Φ)	ptr=e	stack=c
	stack=c,N	fun(e)	ptr=i
	stack=c,N,N	fun(i)	ptr=NULL
	fun(Φ)	ptr=N	stack=c,N
	fun(Φ)	ptr=N	stack=c
	fun(Φ)	ptr=c	stack=
	stack=f	fun(c)	ptr=NULL
	fun(Φ)	ptr=f	stack=
	stack=N	fun(f)	ptr=NULL
	fun(Φ)	ptr=N	stack=
	end
	fun(Φ)
	a,b,d,g,Φ,Φ,h,Φ,Φ,e,i,Φ,Φ,Φ,c,Φ,f,Φ,Φ
	*/
	template <template<typename U>class List, template<typename>class Stack>
	List<TreeNodeOrder<T>> _preOrderIterativePrintWithNode() {
		List<TreeNodeOrder<T>> list;
		Stack<BTree*> stack;
		BTree* ptr = this;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				list.push_back(TreeNodeOrder<T>(ptr->element, false)); // 放入已访问的节点
				stack.push(ptr->rChild); // 右子树入栈
				ptr = ptr->lChild; // 访问左节点
			}
			else {
				list.push_back(TreeNodeOrder<T>()); // 空节点
				ptr = stack.top();
				stack.pop();
			}
		}
		list.push_back(TreeNodeOrder<T>());//最后的空节点
		return list;
	}
	template <template<typename U>class List, template<typename>class Stack>
	List<T> _preOrderIterativePrint() {
		List<T> list;
		Stack<BTree*> stack;
		BTree* ptr = this;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				list.push_back(ptr->element);//放入本节点
				stack.push(ptr->rChild); // 右子树入栈
				ptr = ptr->lChild; // 访问左节点
			}
			else {//空节点,回退栈
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
				ptr = ptr->lChild;//访问左节点
			}
			else {//空节点,回退到栈顶访问其右节点
				ptr = stack.top();
				list.push_back(ptr->element);
				stack.pop();//该节点子树将全部遍历,退栈
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
				ptr = ptr->lChild;//访问左节点
			}
			else {//空节点,回退到栈顶访问其右节点
				prev = stack.top();
				if (ptr == prev->rChild) {//该节点左右子树搜索完毕,检测本节点
					ptr = prev;
					list.push_back(ptr->element);
					stack.pop();//本节点检查完毕,退栈
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
	ptr=c	fun(c)	queue=d,e,Φ,f
	ptr=d	fun(d)	queue=e,Φ,f,g,h
	ptr=e	fun(e)	queue=Φ,f,g,h,i,Φ
	ptr=Φ	queue=f,g,h,i,Φ	ptr=f	fun(f)	queue=g,h,i,Φ,Φ,Φ
	ptr=g	queue=h,i,Φ,Φ,Φ,Φ,Φ
	ptr=h	queue=i,Φ,Φ,Φ,Φ,Φ,Φ,Φ
	ptr=i	queue=Φ,Φ,Φ,Φ,Φ,Φ,Φ,Φ,Φ
	ptr=Φ	queue=Φ,Φ,Φ,Φ,Φ,Φ,Φ,Φ	ptr=Φ	queue=Φ,Φ,Φ,Φ,Φ,Φ,Φ	ptr=Φ	queue=Φ,Φ,Φ,Φ,Φ,Φ
	ptr=Φ	queue=Φ,Φ,Φ,Φ,Φ	ptr=Φ	queue=Φ,Φ,Φ,Φ	ptr=Φ	queue=Φ,Φ,Φ	ptr=Φ	queue=Φ,Φ	ptr=Φ	queue=Φ
	ptr=Φ	queue=	end
	a,b,c,d,e,Φ,f,g,h,i,Φ,Φ,Φ,Φ,Φ,Φ,Φ,Φ,Φ
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
	//使用节点序列构建树
	/*
				a
			b		c
		d	e			f
	   g h i
	A,B,D,G,Φ,Φ,H,Φ,Φ,E,I,Φ,Φ,Φ,C,Φ,F,Φ,Φ this简称T	stack简称S	ptr简称P	prev简称V
	A S=A V=A P=AnewL 	B S=A,B V=B P=BnewL 	D S=A,B,D V=D P=DnewL	G S=A,B,D,G V=G P=GnewL
	Φ (P=V->L) DetP V=G P=V->R=GnewR S=A,B,D
	Φ (P=V->R) DetP P=G V=Nul (V=P->R) {V=G P=D S=A,B} V=D P=DnewR
	H S=A,B,H V=H P=HnewL
	Φ (P=V->L) DetP V=H P=V->R=HnewR S=A,B
	Φ (P=V->R) DetP P=H V=Nul (V=P->R) {V=H P=B S=A} V=B P=BnewR
	E S=A,E V=E P=EnewL		I S=A,E,I V=I P=InewL
	Φ (P=V->L) DetP V=I P=V->R=InewR S=A,E
	Φ (P=V->R) DetP P=I V=Nul (V=P->R) {V=I P=E S=A} V=E P=EnewR
	Φ (P=V->R) DetP P=E V=Nul (V=P->R) {V=E P=A S=} V=A P=AnewR
	C S=C V=C P=CnewL
	Φ (P=V->L) DetP V=C P=V->R=CnewR S=
	F S=F V=F P=FnewL
	Φ (P=V->L) DetP V=F P=V->R=FnewR S=
	Φ (P=V->R) DetP P=F P=Null (V=P->R) {return}
	*/
	template <template<typename U>class List, template<typename>class Stack>
	void _preOrderIterativeBuildWithNode(const List<TreeNodeOrder<T>>& order) {
		Stack<BTree*> stack;
		BTree* ptr = this, * prev = nullptr;
		for (size_t i = 0; i < order.size(); ++i) {
			if (!order[i].isEmpty) {//P=V->L或P=V->R向P=V->L状态转移
				ptr->element = order[i].element;
				stack.push(ptr);
				ptr->lChild = new BTree();
				ptr->rChild = new BTree();
				prev = ptr;
				ptr = ptr->lChild;
			}
			else {
				if (prev->lChild == ptr) {//左子树经过后赋空,向右子树转移P=V->L向P=V->R状态转移
					delete prev->lChild;
					prev->lChild = nullptr;
					ptr = prev->rChild;
					stack.pop();
				}
				else {//右子树经过后赋空,开始回溯P=V->R向V=P->R状态转移
					delete prev->rChild;
					prev->rChild = nullptr;
					ptr = prev;
					prev = ptr->rChild;
					while (ptr->rChild == prev) {//回溯至节点为左子树,V=P->R向V=P->R,V=P->L转移
						if (stack.empty())
							return;
						prev = ptr;
						ptr = stack.top();
						stack.pop();
					}
					prev = ptr;//左子树经过后遍历右子树V=P->L向P=V->R转移
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
		for (int count = 0; i != end || count == 1; ++i) {//[左树,分割点]:节点=空节点+1
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
	//对所有节点进行Function操作
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
				func(ptr);//放入本节点
				stack.push(ptr->rChild); // 右子树入栈
				ptr = ptr->lChild; // 访问左节点
			}
			else {//空节点,回退栈
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
				ptr = ptr->lChild;//访问左节点
			}
			else {//空节点,回退到栈顶访问其右节点
				ptr = stack.top();
				func(ptr);//中序搜索本节点
				stack.pop();//该节点子树将全部遍历,退栈
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
				ptr = ptr->lChild;//访问左节点
			}
			else {//空节点,回退到栈顶访问其右节点
				if (stack.empty())
					return;
				prev = stack.top();
				if (ptr == prev->rChild) {//该节点左右子树搜索完毕,检测本节点
					ptr = prev;
					func(ptr);
					stack.pop();//本节点检查完毕,退栈
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
	//查找符合Condition谓词的第一个对象
	template <typename Condition>
	BTree* _preOrderRecursiveWithCondition(Condition&& cond) {
		BTree* that = nullptr;
		if (cond(this) && that == nullptr)
			that = this;//可以直接return this 但是这样很整齐
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
				if (cond(ptr))//先序搜索本节点
					return ptr;
				stack.push(ptr->rChild); // 右子树入栈
				ptr = ptr->lChild; // 访问左节点
			}
			else {//空节点,回退栈
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
				ptr = ptr->lChild;//访问左节点
			}
			else {//空节点,回退到栈顶访问其右节点
				ptr = stack.top();
				if (cond(ptr))//中序搜索本节点
					return ptr;
				stack.pop();//该节点子树将全部遍历,退栈
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
				ptr = ptr->lChild;//访问左节点
			}
			else {//空节点,回退到栈顶访问其右节点
				prev = stack.top();
				if (ptr == prev->rChild) {//该节点左右子树搜索完毕,检测本节点
					ptr = prev;
					if (cond(ptr)) {
						return ptr;
					}
					stack.pop();//本节点检查完毕,退栈
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
	//对节点进行condition判断，如果为true 对元素执行function
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
				if (cond(ptr))//先序搜索本节点
					func(ptr);
				stack.push(ptr->rChild);
				ptr = ptr->lChild;//访问左节点
			}
			else {//空节点,回退到栈顶访问其右节点
				ptr = stack.top();
				stack.pop();//该节点子树将全部遍历,退栈
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
				ptr = ptr->lChild;//访问左节点
			}
			else {//空节点,回退到栈顶访问其右节点
				ptr = stack.top();
				if (cond(ptr))//中序搜索本节点
					func(ptr);
				stack.pop();//该节点子树将全部遍历,退栈
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
				ptr = ptr->lChild;//访问左节点
			}
			else {//空节点,回退到栈顶访问其右节点
				prev = stack.top();
				if (ptr == prev->rChild) {//该节点左右子树搜索完毕,检测本节点
					ptr = prev;
					if (cond(ptr))
						func(ptr);
					stack.pop();//本节点检查完毕,退栈
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
