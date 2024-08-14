#pragma once
#include "TheTreeHead.h"
template<typename T>
class BinaryWithParentTree {
public:
	using BWPTree = BinaryWithParentTree<T>;
	using Function = std::function<void(BWPTree*)>;
	using Condition = std::function<bool(BWPTree*)>;
protected:
	T element;//内容元素
	BWPTree* parent = nullptr;//双亲节点 简称父节点
	BWPTree* lChild = nullptr;//左子节点 简称左节点
	BWPTree* rChild = nullptr;//右子节点 简称右节点
public:
	//仅本节点内容 无子节点 无父节点
	BinaryWithParentTree(const T& in)
		:BWPTree(in) {}
	//仅子节点内容
	BinaryWithParentTree(const T& left, const T& right)
		:lChild(new BWPTree(left)), rChild(new BWPTree(right)) {
		lChild->parent = this;
		rChild->parent = this;
	}
	//本节点与子节点内容
	BinaryWithParentTree(const T& in, const T& left, const T& right)
		:lChild(new BWPTree(left)), rChild(new BWPTree(right)), element(in) {
		lChild->parent = this;
		rChild->parent = this;
	}
	//仅父节点链接
	BinaryWithParentTree(const T& in, const BWPTree* inParent)
		:element(in), parent(inParent) {}
	//仅子节点链接
	BinaryWithParentTree(const T& in, const BWPTree* inLeft, const BWPTree* inRight)
		:element(in), lChild(inLeft), rChild(inRight) {}
	//三节点链接 先父节点 后子节点
	BinaryWithParentTree(const T& in, const BWPTree* inParent, const BWPTree* inLeft, const BWPTree* inRight)
		:element(in), parent(inParent), lChild(inLeft), rChild(inRight) {}
	//以某种遍历形式构建完全二叉树
	template <template<typename U>class List, template<typename V>class Stack>
	BinaryWithParentTree(const List<TreeNodeOrder<T>>& order, int model) {

	}
	//靠前中后序遍历构建二叉树
	BinaryTree(BWPTree** orderA, BWPTree** orderB, size_t length, int model) {

	}
	//复制树
	BinaryWithParentTree(const BWPTree& other) :element(other.element), parent(nullptr) {
		if (other.lChild != nullptr) {
			lChild = new BWPTree(*(other.lChild));
			lChild->parent = this;
		}
		if (other.rChild != nullptr) {
			rChild = new BWPTree(*(other.rChild));
			rChild->parent = this;
		}
	}
	//移动树
	BinaryWithParentTree(BWPTree&& other)
		:parent(other.parent), lChild(other.lChild), rChild(other.rChild), element(std::move(other.element)) {
		if (other.parent) {
			if (other.parent->lChild == &other) {
				other.parent->lChild = this;
			}
			if (other.parent->rChild == &other) {
				other.parent->rChild = this;
			}
			if (other.parent->lChild != &other && other.parent->rChild != &other) {
				parent = nullptr;
			}
			other.parent = nullptr;
		}
		other.lChild = nullptr;
		other.rChild = nullptr;
	}
	//删除树
	~BinaryWithParentTree() {
		if (parent) {
			if (parent->lChild == this) {
				parent->lChild = nullptr;
			}
			if (parent->rChild == this) {
				parent->rChild = nullptr;
			}
		}
		delete lChild;
		delete rChild;
	}
	//拷贝赋值函数
	BWPTree& operator=(const BWPTree& other) {
		if (this != &other) {
			delete lChild;
			delete rChild;
			if (other.lChild != nullptr)
				lChild = new BWPTree(*(other.lChild));
			else
				lChild = nullptr;
			if (other.rChild != nullptr)
				rChild = new BWPTree(*(other.rChild));
			else
				rChild = nullptr;
			element = other.element;
		}
		return *this;
	}
	//移动赋值函数
	BWPTree& operator=(BWPTree&& other)noexcept {
		if (this != &other) {
			delete lChild;
			delete rChild;
			if (other.parent) {
				if (other.parent->lChild == &other) {
					other.parent->lChild = nullptr;
				}
				if (other.parent->rChild == &other) {
					other.parent->rChild = nullptr;
				}
				other.parent = nullptr;
			}
			lChild = other.lChild;
			rChild = other.rChild;
			lChild->parent = this;
			rChild->parent = this;
			other.lChild = nullptr;
			other.rChild = nullptr;
			element = std::move(other.element);
		}
		return *this;
	}
	//断连并清除节点
	inline static void deleteNode(BWPTree*& other) {
		if (!other)
			return;
		if (other->parent) {//断连该节点
			if (other->parent->lChild == other)
				other->parent->lChild = nullptr;
			if (other->parent->rChild == other)
				other->parent->rChild = nullptr;
			other->parent = nullptr;
		}
		if (other->lChild) {
			other->lChild->parent = nullptr;
			other->lChild = nullptr;
		}
		if (other->rChild) {
			other->rChild->parent = nullptr;
			other->rChild = nullptr;
		}
		delete other;
		other = nullptr;
	}
	//通过先序非递归遍历清理树
	template <template<typename V>class Stack>
	void _preClearIterative() const {
		Stack<BWPTree*> stack;
		BWPTree* ptr = this, * prev = nullptr;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				prev = ptr;
				stack.push(ptr->rChild); // 右子树入栈
				ptr = ptr->lChild; // 访问左节点
				prev->parent = nullptr;//断连该节点
				prev->lChild = nullptr;
				prev->rChild = nullptr;
				delete prev;
			}
			else {//空节点,回退栈
				ptr = stack.top();
				stack.pop();
			}
		}
		return nullptr;
	}
	//通过中序非递归遍历清理树
	template <template<typename V>class Stack>
	void _inClearIterative() {
		Stack<BWPTree*> stack;
		BWPTree* ptr = this, * prev = nullptr;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				stack.push(ptr);
				ptr = ptr->lChild; // 访问左节点
			}
			else {//空节点,回退到栈顶访问其右节点
				prev = stack.top();
				stack.pop();//该节点子树将全部遍历,退栈
				ptr = prev->rChild;
				prev->parent = nullptr;//断连该节点
				prev->lChild = nullptr;
				prev->rChild = nullptr;
				delete prev;
			}
		}
	}
	//通过后序非递归遍历清理树
	template <template<typename V>class Stack>
	void _postClearIterative() {
		Stack<BWPTree*> stack;
		BWPTree* ptr = this, * prev = nullptr;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr && ptr != prev) {
				stack.push(ptr);
				ptr = ptr->lChild;//访问左节点
			}
			else {//空节点,回退到栈顶访问其右节点
				prev = stack.top();
				if (ptr == prev->rChild) {//该节点左右子树搜索完毕,检测本节点
					ptr = prev;
					if (prev != this) {//断连该节点
						prev->parent = nullptr;
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
	//返回左节点指针
	inline BWPTree* getLeftChildLink() { return lChild; }
	inline const BWPTree* getLeftChildLink()const { return lChild; }
	//返回右节点指针
	inline BWPTree* getRightChildLink() { return rChild; }
	inline const BWPTree* getRightChildLink()const { return rChild; }
	//返回父节点指针
	inline BWPTree* getParentLink() { return parent; }
	inline const BWPTree* getParentLink()const { return parent; }
	//返回兄节点指针
	BWPTree* gatSiblingLink() {
		if (parent) {
			if (parent->lChild == this && parent->rChild != this) {
				return parent->rChild;
			}
			else if (parent->lChild != this && parent->rChild == this) {
				return parent->lChild;
			}
			else if (parent->lChild != this && parent->rChild != this) {
				parent = nullptr;
				return nullptr;
			}
			else {
				parent->lChild = nullptr;
				parent->rChild = nullptr;
				parent = nullptr;
				return nullptr;
			}
		}
		return nullptr;
	}
	const BWPTree* gatSiblingLink()const {
		if (parent) {
			if (parent->lChild == this && parent->rChild != this) {
				return parent->rChild;
			}
			else if (parent->lChild != this && parent->rChild == this) {
				return parent->lChild;
			}
			else if (parent->lChild != this && parent->rChild != this) {
				parent = nullptr;
				return nullptr;
			}
			else {
				parent->lChild = nullptr;
				parent->rChild = nullptr;
				parent = nullptr;
				return nullptr;
			}
		}
		return nullptr;
	}
	//返回本节点内容
	inline T& getElement() { return element; }
	inline const T& getElement()const { return element; }
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
	//生成左子节点并赋值 默认无左子节点
	BWPTree* _createLeftChild(const T& value) {
		lChild = new BWPTree(value, this);
		return lChild;
	}
	//生成右子节点并赋值
	BWPTree* _createRightChild(const T& value) {
		rChild = new BWPTree(value, this);
		return rChild;
	}
	//移除左子节点及其树
	void clearLeftChild() {
		delete lChild;
	}
	//移除右值节点及其树
	void clearRightChild() {
		delete rChild;
	}
	//连接左子节点
	void _connectLeftChild(BWPTree* left) {
		lChild = left;
		lChild->parent = this;
	}
	void _connectLeftChild(BWPTree& left) {
		lChild = &left;
		lChild->parent = this;
	}
	//连接右子节点
	void _connectRightChild(BWPTree* right) {
		rChild = right;
		rChild->parent = this;
	}
	void _connectRightChild(BWPTree& right) {
		rChild = &right;
		rChild->parent = this;
	}
	//断连左子节点
	BWPTree* _disconnectLeftChild() {
		if (lChild) {
			BWPTree* temp = lChild;
			lChild->parent = nullptr;
			lChild = nullptr;
			return temp;
		}
		else {
			return nullptr;
		}
	}
	//断连右子节点
	BWPTree* _disconnectRightChild() {
		if (rChild) {
			BWPTree* temp = rChild;
			rChild->parent = nullptr;
			rChild = nullptr;
			return temp;
		}
		else {
			return nullptr;
		}
	}
	//断连父节点
	BWPTree* _disconnectParent() {
		if (parent) {
			BWPTree* temp = parent;
			if (parent->lChild == this) {
				parent->lChild = nullptr;
			}
			if (parent->rChild == this) {
				parent->rChild = nullptr;
			}
			parent = nullptr;
			return temp;
		}
		else {
			return nullptr;
		}
	}
	//断连全节点
	void _disconnect() {
		if (lChild) {
			lChild->parent = nullptr;
			lChild = nullptr;
		}
		if (rChild) {
			rChild->parent = nullptr;
			rChild = nullptr;
		}
		if (parent) {
			if (parent->lChild == this) {
				parent->lChild = nullptr;
			}
			if (parent->rChild == this) {
				parent->rChild = nullptr;
			}
			parent = nullptr;
		}
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
	void rotateLeft() {
		if (rChild == nullptr) {
			return;
		}
		else {
			BWPTree* temp = rChild;
			if (parent) {
				if (parent->lChild == this) {
					parent->lChild = temp;
					temp->parent = parent;
				}
				if (parent->rChild == this) {
					parent->rChild = temp;
					temp->parent = parent;
				}
			}
			if (temp->lChild) {
				rChild = temp->lChild;
				temp->lChild->parent = this;
			}
			else {
				rChild = nullptr;
			}
			temp->lChild = this;
			parent = temp;
		}
	}
	static void rotateLeft(BWPTree*& root) {
		if (root == nullptr || root->rChild == nullptr) {
			return;
		}
		else {
			BWPTree* temp = root->rChild;
			if (root->parent) {
				if (root->parent->lChild == root) {
					root->parent->lChild = temp;
					temp->parent = root->parent;
				}
				if (root->parent->rChild == root) {
					root->parent->rChild = temp;
					temp->parent = root->parent;
				}
			}
			if (temp->lChild) {
				root->rChild = temp->lChild;
				temp->lChild->parent = root;
			}
			else {
				root->rChild = nullptr;
			}
			temp->lChild = root;
			root->parent = temp;
			root = temp;
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
	void rotateRight() {
		if (lChild == nullptr) {
			return;
		}
		else {
			BWPTree* temp = lChild;
			if (parent) {
				if (parent->lChild == this) {
					parent->lChild = temp;
					temp->parent = parent;
				}
				if (parent->rChild == this) {
					parent->rChild = temp;
					temp->parent = parent;
				}
			}
			if (temp->rChild) {
				lChild = temp->rChild;
				temp->rChild->parent = this;
			}
			else {
				lChild = nullptr;
			}
			temp->rChild = this;
			parent = temp;
		}
	}
	static void rotateRight(BWPTree*& root) {
		if (root == nullptr || root->lChild == nullptr) {
			return;
		}
		else {
			BWPTree* temp = root->lChild;
			if (root->parent) {
				if (root->parent->lChild == root) {
					root->parent->lChild = temp;
					temp->parent = root->parent;
				}
				if (root->parent->rChild == root) {
					root->parent->rChild = temp;
					temp->parent = root->parent;
				}
			}
			if (temp->rChild) {
				root->lChild = temp->rChild;
				temp->rChild->parent = root;
			}
			else {
				root->lChild = nullptr;
			}
			temp->rChild = root;
			root->parent = temp;
			root = temp;
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
	/*
				a
			b		c
		d	e			f
	   g h i
	P->a V->N t=1
	P->b V->a t=2
	P->d V->b t=3
	P->g V->d t=4
	P->d V->g t=3
	P->h V->d t=4
	P->d V->h t=3
	P->b V->d t=2
	P->e V->d t=3
	P->i V->e t=4
	P->e V->i t=3
	P->b V->e t=2
	P->a V->b t=1
	P->c V->a t=2
	P->f V->c t=3
	P->c V->f t=2
	P->a V->c t=1
	P->N V->a
	end
	*/
	size_t depthIterative() const {
		size_t maxDepth = 1, tempDepth = 1;
		BWPTree* ptr = this;
		BWPTree* prev = nullptr;
		BWPTree* tempParent = parent;
		parent = nullptr;
		while (ptr != nullptr) {
			if (ptr->lChild != nullptr && prev != ptr->lChild && prev != ptr->rChild) {//访问左节点
				++tempDepth;
				maxDepth = maxDepth > tempDepth ? maxDepth : tempDepth;
				prev = ptr;
				ptr = ptr->lChild;
			}
			else if (ptr->rChild != nullptr && prev != ptr->rChild) {//访问右节点
				++tempDepth;
				maxDepth = maxDepth > tempDepth ? maxDepth : tempDepth;
				prev = ptr;
				ptr = ptr->rChild;
			}
			else {//回溯
				prev = ptr;
				ptr = ptr->parent;
				--tempDepth;
			}
		}
		parent = tempParent;
		return maxDepth;
	}
	//元素数量
	size_t sizeRecursive() const {
		size_t size = 1;
		Function fun = Function(
			[&size](BWPTree* node) {
				++size;
			});
		_preOrderRecursiveWithFunctionTraverse(fun);
		return size;
	}
	size_t sizeIterative() const {
		size_t size = 1;
		Function fun = Function(
			[&size](BWPTree* node) {
				++size;
			});
		_preOrderIterativeWithFunctionTraverse(fun);
		return size;
	}
	//对称树
	void symmetryRecursive() {
		Function fun = Function(
			[](BWPTree* node) {
				BWPTree* left = node->lChild;
				BWPTree* right = node->rChild;
				node->rChild = left;
				node->lChild = right;
			});
		_preOrderRecursiveWithFunctionTraverse(fun);
		return;
	}
	void symmetryIterative() {
		Function fun = Function(
			[](BWPTree* node) {
				BWPTree* left = node->lChild;
				BWPTree* right = node->rChild;
				node->rChild = left;
				node->lChild = right;
			});
		_preOrderIterativeWithFunctionTraverse(fun);
		return;
	}
	template <template<typename>class Stack>
	BWPTree* find(const T& value, int model = 0) {
		Condition conditionFun = Condition(
			[&value](BWPTree* node)->bool {return node->getElement() == value; });
		return orderCondition<Stack>(conditionFun, model);
	}
	//进行condition判断，如果为true返回节点
	template <template<typename>class Stack>
	BWPTree* orderCondition(Condition&& cond, int model = 0) {
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
	Recursive代表递归操作  Iterative代表非递归操作	WithStack代表额外使用栈
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
	template <template<typename U>class List, template<typename V>class Stack>
	List<TreeNodeOrder<T>> _preOrderIterativePrintWithStackNode() {
		List<TreeNodeOrder<T>> list;
		Stack<BWPTree*> stack;
		BWPTree* ptr = this;
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
	template <template<typename U>class List, template<typename V>class Stack>
	List<T> _preOrderIterativePrintWithStack() {
		List<T> list;
		Stack<BWPTree*> stack;
		BWPTree* ptr = this;
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
	template <template<typename U>class List, template<typename V>class Stack>
	List<T> _inOrderIterativePrintWithStack() {
		List<T> list;
		Stack<BWPTree*> stack;
		BWPTree* ptr = this;
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
	template <template<typename U>class List, template<typename V>class Stack>
	List<T> _postOrderIterativePrintWithStack() {
		List<T> list;
		Stack<BWPTree*> stack;
		BWPTree* ptr = this, * prev = nullptr;
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
	template <template<typename U>class List, template<typename V>class Queue>
	List<TreeNodeOrder<T>> _levelOrderIterativePrintWithNode() {
		List<TreeNodeOrder<T>> list;
		Queue<T> queue;
		BWPTree* ptr = this;
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
	template <template<typename U>class List, template<typename V>class Queue>
	List<T> _levelOrderIterativePrint() {
		List<T> list;
		Queue<BWPTree*> queue;
		BWPTree* ptr = this;
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
	/*
				a
			b		c
		d	e			f
	   g h i
	P=a	V=N	1	f(b)
	P=b	V=a	1	f(d)
	P=d	V=b	1	f(g)
	P=g	V=d	3
	P=d	V=g	2	f(h)
	P=h	V=d	3
	P=d	V=h	3
	P=b	V=d	2	f(e)
	P=e	V=d	1	f(i)
	P=i	V=e	3
	P=e	V=i	3
	P=b	V=e	3
	P=a	V=b	2	f(c)
	P=c	V=a	2	f(f)
	P=f	V=c	3
	P=c	V=f	3
	P=a	V=c	3
	P=N	V=a	end
	*/
	template <template<typename U>class List>
	List<T> _preOrderIterativePrint() {
		List<T> list;
		BWPTree* ptr = this;
		BWPTree* prev = nullptr;
		BWPTree* tempParent = parent;
		parent = nullptr;
		list.push_back(ptr->element);
		while (ptr != nullptr) {
			if (ptr->lChild != nullptr && prev != ptr->lChild && prev != ptr->rChild) {//访问左节点
				prev = ptr;
				ptr = ptr->lChild;
				list.push_back(ptr->element);
			}
			else if (ptr->rChild != nullptr && prev != ptr->rChild) {//访问右节点
				prev = ptr;
				ptr = ptr->rChild;
				list.push_back(ptr->element);
			}
			else {//回溯
				prev = ptr;
				ptr = ptr->parent;
			}
		}
		parent = tempParent;
		return list;
	}
	/*
				a
			b		c
		d	e			f
	   g h i
	P=a	V=N	1
	P=b	V=a	1
	P=d	V=b	1
	P=g	V=d	3	f(g)
	P=d	V=g	2	f(d)
	P=h	V=d	3	f(h)
	P=d	V=h	3
	P=b	V=d	2	f(b)
	P=e	V=d	1
	P=i	V=e	3	f(i)
	P=e	V=i	3	f(e)
	P=b	V=e	3
	P=a	V=b	2	f(a)
	P=c	V=a	2	f(c)
	P=f	V=c	3	f(f)
	P=c	V=f	3
	P=a	V=c	3
	P=N	V=a	end
	*/
	template <template<typename U>class List>
	List<T> _inOrderIterativePrint() {
		List<T> list;
		BWPTree* ptr = this;
		BWPTree* prev = nullptr;
		BWPTree* tempParent = parent;
		parent = nullptr;
		while (ptr != nullptr) {
			if (ptr->lChild != nullptr && prev != ptr->lChild && prev != ptr->rChild) {//访问左节点
				prev = ptr;
				ptr = ptr->lChild;
			}
			else if (ptr->rChild != nullptr && prev != ptr->rChild) {//访问右节点
				list.push_back(ptr->element);
				prev = ptr;
				ptr = ptr->rChild;
			}
			else {//回溯
				if (ptr->rChild = nullptr) {
					list.push_back(ptr->element);
				}
				prev = ptr;
				ptr = ptr->parent;
			}
		}
		parent = tempParent;
		return list;
	}
	/*
				a
			b		c
		d	e			f
	   g h i
	P=a	V=N	1
	P=b	V=a	1
	P=d	V=b	1
	P=g	V=d	3	f(g)
	P=d	V=g	2
	P=h	V=d	3	f(h)
	P=d	V=h	3	f(d)
	P=b	V=d	2
	P=e	V=d	1
	P=i	V=e	3	f(i)
	P=e	V=i	3	f(e)
	P=b	V=e	3	f(b)
	P=a	V=b	2
	P=c	V=a	2
	P=f	V=c	3	f(f)
	P=c	V=f	3	f(c)
	P=a	V=c	3	f(a)
	P=N	V=a	end
	*/
	template <template<typename U>class List>
	List<T> _postOrderIterativePrint() {
		List<T> list;
		BWPTree* ptr = this;
		BWPTree* prev = nullptr;
		BWPTree* tempParent = parent;
		parent = nullptr;
		while (ptr != nullptr) {
			if (ptr->lChild != nullptr && prev != ptr->lChild && prev != ptr->rChild) {//访问左节点
				prev = ptr;
				ptr = ptr->lChild;
			}
			else if (ptr->rChild != nullptr && prev != ptr->rChild) {//访问右节点
				prev = ptr;
				ptr = ptr->rChild;
			}
			else {//回溯
				list.push_back(ptr->element);
				prev = ptr;
				ptr = ptr->parent;
			}
		}
		parent = tempParent;
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
	template <template<typename U>class List, template<typename V>class Stack>
	void _preOrderIterativeBuildWithNode(const List<TreeNodeOrder<T>>& order) {
		Stack<BWPTree*> stack;
		BWPTree* ptr = this, * prev = nullptr;
		for (size_t i = 0; i < order.size(); ++i) {
			if (!order[i].isEmpty) {//P=V->L或P=V->R向P=V->L状态转移
				ptr->element = order[i].element;
				stack.push(ptr);
				ptr->lChild = new BWPTree();
				ptr->rChild = new BWPTree();
				ptr->lChild->parent = ptr;
				ptr->rChild->parent = ptr;
				prev = ptr;
				ptr = ptr->lChild;
			}
			else {
				if (prev->lChild == ptr) {//左子树经过后赋空,向右子树转移P=V->L向P=V->R状态转移
					delete prev->lChild;
					ptr = prev->rChild;
					stack.pop();
				}
				else {//右子树经过后赋空,开始回溯P=V->R向V=P->R状态转移
					delete prev->rChild;
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
	template <template<typename U>class List, template<typename V>class Stack>
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
	template <template<typename U>class List, template<typename V>class Queue>
	void _levelOrderIterativeBuildWithNode(const List<TreeNodeOrder<T>>& order) {
		struct Node {
			BWPTree* ptr;
			bool isleft;
			Node(BWPTree* inPtr = nullptr, bool inIsLeft = 0) :ptr(inPtr), isleft(inIsLeft) {};
		};
		Queue<Node> queue;
		BWPTree* tempptr = this;
		bool tempisleft;
		tempptr = order[0].element;
		queue.push(Node(tempptr, 1));
		queue.push(Node(tempptr, 0));
		for (size_t i = 1; i < order.size(); ++i) {
			tempptr = queue.back().ptr;
			tempisleft = queue.back().isleft;
			if (tempisleft == 1) {
				if (!order[i].isEmpty()) {
					tempptr->lChild = new BWPTree(List[i].element);
					queue.push(Node(tempptr, 1));
					queue.push(Node(tempptr, 0));
				}
				else {
					tempptr->lChild = nullptr;
				}
			}
			else {
				if (!order[i].isEmpty()) {
					tempptr->rChild = new BWPTree(List[i].element);
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
	void _preOrderIterativeWithStackFunctionTraverse(Function&& func) {
		Stack<BWPTree*> stack;
		BWPTree* ptr = this;
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
	void _inOrderIterativeWithStackFunctionTraverse(Function&& func) {
		Stack<BWPTree*> stack;
		BWPTree* ptr = this;
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
	void _postOrderIterativeWithStackFunctionTraverse(Function&& func) {
		Stack<BWPTree*> stack;
		BWPTree* ptr = this, * prev = nullptr;
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
		Queue<BWPTree*> queue;
		BWPTree* ptr = this;
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
	template <typename Function>
	void _preOrderIterativeWithFunctionTraverse(Function&& func) {
		BWPTree* ptr = this;
		BWPTree* prev = nullptr;
		BWPTree* tempParent = parent;
		parent = nullptr;
		func(ptr);
		while (ptr != nullptr) {
			if (ptr->lChild != nullptr && prev != ptr->lChild && prev != ptr->rChild) {//访问左节点
				prev = ptr;
				ptr = ptr->lChild;
				func(ptr);
			}
			else if (ptr->rChild != nullptr && prev != ptr->rChild) {//访问右节点
				prev = ptr;
				ptr = ptr->rChild;
				func(ptr);
			}
			else {//回溯
				prev = ptr;
				ptr = ptr->parent;
			}
		}
		parent = tempParent;
	}
	template <typename Function>
	void _inOrderIterativeWithFunctionTraverse(Function&& func) {
		BWPTree* ptr = this;
		BWPTree* prev = nullptr;
		BWPTree* tempParent = parent;
		parent = nullptr;
		while (ptr != nullptr) {
			if (ptr->lChild != nullptr && prev != ptr->lChild && prev != ptr->rChild) {//访问左节点
				prev = ptr;
				ptr = ptr->lChild;
			}
			else if (ptr->rChild != nullptr && prev != ptr->rChild) {//访问右节点
				func(ptr);
				prev = ptr;
				ptr = ptr->rChild;
			}
			else {//回溯
				if (ptr->rChild = nullptr) {
					func(ptr);
				}
				prev = ptr;
				ptr = ptr->parent;
			}
		}
		parent = tempParent;
	}
	template <typename Function>
	void _postOrderIterativeWithFunctionTraverse(Function&& func) {
		BWPTree* ptr = this;
		BWPTree* prev = nullptr;
		BWPTree* tempParent = parent;
		parent = nullptr;
		while (ptr != nullptr) {
			if (ptr->lChild != nullptr && prev != ptr->lChild && prev != ptr->rChild) {//访问左节点
				prev = ptr;
				ptr = ptr->lChild;
			}
			else if (ptr->rChild != nullptr && prev != ptr->rChild) {//访问右节点
				prev = ptr;
				ptr = ptr->rChild;
			}
			else {//回溯
				func(ptr);
				prev = ptr;
				ptr = ptr->parent;
			}
		}
		parent = tempParent;
	}
	//查找符合Condition谓词的第一个对象
	template <typename Condition>
	BWPTree* _preOrderRecursiveWithCondition(Condition&& cond) const {
		BWPTree* that = nullptr;
		if (cond(this) && that == nullptr)
			that = this;//可以直接return this 但是这样很整齐
		if (lChild != nullptr && that == nullptr)
			that = lChild->_preOrderRecursiveWithCondition(cond);
		if (rChild != nullptr && that == nullptr)
			that = rChild->_preOrderRecursiveWithCondition(cond);
		return that;
	}
	template <typename Condition>
	BWPTree* _inOrderRecursiveWithCondition(Condition&& cond) const {
		BWPTree* that = nullptr;
		if (lChild != nullptr && that == nullptr)
			that = lChild->_inOrderRecursiveWithCondition(cond);
		if (cond(this) && that == nullptr)
			that = this;
		if (rChild != nullptr && that == nullptr)
			that = rChild->_inOrderRecursiveWithCondition(cond);
		return that;
	}
	template <typename Condition>
	BWPTree* _postOrderRecursiveWithCondition(Condition&& cond) const {
		BWPTree* that = nullptr;
		if (lChild != nullptr && that == nullptr)
			that = lChild->_postOrderRecursiveWithCondition(cond);
		if (rChild != nullptr && that == nullptr)
			that = rChild->_postOrderRecursiveWithCondition(cond);
		if (cond(this) && that == nullptr)
			that = this;
		return that;
	}
	template <template<typename V>class Stack>
	BWPTree* _preOrderIterativeWithStackCondition(Condition&& cond) const {
		Stack<BWPTree*> stack;
		BWPTree* ptr = this;
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
	template <template<typename V>class Stack>
	BWPTree* _inOrderIterativeWithStackCondition(Condition&& cond) const {
		Stack<BWPTree*> stack;
		BWPTree* ptr = this;
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
	template <template<typename V>class Stack>
	BWPTree* _postOrderIterativeWithStackCondition(Condition&& cond) const {
		Stack<BWPTree*> stack;
		BWPTree* ptr = this, * prev = nullptr;
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
	template <template<typename V>class Queue>
	BWPTree* _levelOrderIterativeWithCondition(Condition&& cond) {
		Queue<BWPTree*> queue;
		BWPTree* ptr = this;
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
	template <typename Condition>
	BWPTree* _preOrderIterativeWithCondition(Condition&& cond) {
		BWPTree* ptr = this;
		BWPTree* prev = nullptr;
		BWPTree* tempParent = parent;
		parent = nullptr;
		if (cond(ptr)) {
			parent = tempParent;
			return ptr;
		}
		while (ptr != nullptr) {
			if (ptr->lChild != nullptr && prev != ptr->lChild && prev != ptr->rChild) {//访问左节点
				prev = ptr;
				ptr = ptr->lChild;
				if (cond(ptr)) {
					parent = tempParent;
					return ptr;
				}
			}
			else if (ptr->rChild != nullptr && prev != ptr->rChild) {//访问右节点
				prev = ptr;
				ptr = ptr->rChild;
				if (cond(ptr)) {
					parent = tempParent;
					return ptr;
				}
			}
			else {//回溯
				prev = ptr;
				ptr = ptr->parent;
			}
		}
		parent = tempParent;
		return nullptr;
	}
	template <typename Condition>
	BWPTree* _inOrderIterativeWithCondition(Condition&& cond) {
		BWPTree* ptr = this;
		BWPTree* prev = nullptr;
		BWPTree* tempParent = parent;
		parent = nullptr;
		while (ptr != nullptr) {
			if (ptr->lChild != nullptr && prev != ptr->lChild && prev != ptr->rChild) {//访问左节点
				prev = ptr;
				ptr = ptr->lChild;
			}
			else if (ptr->rChild != nullptr && prev != ptr->rChild) {//访问右节点
				if (cond(ptr)) {
					parent = tempParent;
					return ptr;
				}
				prev = ptr;
				ptr = ptr->rChild;
			}
			else {//回溯
				if (ptr->rChild = nullptr) {
					if (cond(ptr)) {
						parent = tempParent;
						return ptr;
					}
				}
				prev = ptr;
				ptr = ptr->parent;
			}
		}
		parent = tempParent;
		return nullptr;
	}
	template <typename Condition>
	BWPTree* _postOrderIterativeWithCondition(Condition&& cond) {
		BWPTree* ptr = this;
		BWPTree* prev = nullptr;
		BWPTree* tempParent = parent;
		parent = nullptr;
		while (ptr != nullptr) {
			if (ptr->lChild != nullptr && prev != ptr->lChild && prev != ptr->rChild) {//访问左节点
				prev = ptr;
				ptr = ptr->lChild;
			}
			else if (ptr->rChild != nullptr && prev != ptr->rChild) {//访问右节点
				prev = ptr;
				ptr = ptr->rChild;
			}
			else {//回溯
				if (cond(ptr)) {
					parent = tempParent;
					return ptr;
				}
				prev = ptr;
				ptr = ptr->parent;
			}
		}
		parent = tempParent;
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
	void _inOrderRecursiveWithConditionFunction(Condition&& cond, Function&& func) {
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
		Stack<BWPTree*> stack;
		BWPTree* ptr = this;
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
		Stack<BWPTree*> stack;
		BWPTree* ptr = this;
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
		Stack<BWPTree*> stack;
		BWPTree* ptr = this, * prev = nullptr;
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
		Queue<BWPTree*> queue;
		BWPTree* ptr;
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
	template <typename Condition, typename Function>
	void _preOrderIterativeWithConditionFunction(Condition&& cond, Function&& func) {
		BWPTree* ptr = this;
		BWPTree* prev = nullptr;
		BWPTree* tempParent = parent;
		parent = nullptr;
		if (cond(ptr)) {
			func(ptr);
		}
		while (ptr != nullptr) {
			if (ptr->lChild != nullptr && prev != ptr->lChild && prev != ptr->rChild) {//访问左节点
				prev = ptr;
				ptr = ptr->lChild;
				if (cond(ptr)) {
					func(ptr);
				}
			}
			else if (ptr->rChild != nullptr && prev != ptr->rChild) {//访问右节点
				prev = ptr;
				ptr = ptr->rChild;
				if (cond(ptr)) {
					func(ptr);
				}
			}
			else {//回溯
				prev = ptr;
				ptr = ptr->parent;
			}
		}
		parent = tempParent;
	}
	template <typename Condition, typename Function>
	void _inOrderIterativeWithConditionFunction(Condition&& cond, Function&& func) {
		BWPTree* ptr = this;
		BWPTree* prev = nullptr;
		BWPTree* tempParent = parent;
		parent = nullptr;
		while (ptr != nullptr) {
			if (ptr->lChild != nullptr && prev != ptr->lChild && prev != ptr->rChild) {//访问左节点
				prev = ptr;
				ptr = ptr->lChild;
			}
			else if (ptr->rChild != nullptr && prev != ptr->rChild) {//访问右节点
				if (cond(ptr)) {
					func(ptr);
				}
				prev = ptr;
				ptr = ptr->rChild;
			}
			else {//回溯
				if (ptr->rChild = nullptr) {
					if (cond(ptr)) {
						func(ptr);
					}
				}
				prev = ptr;
				ptr = ptr->parent;
			}
		}
		parent = tempParent;
		return nullptr;
	}
	template <typename Condition, typename Function>
	void _postOrderIterativeWithConditionFunction(Condition&& cond, Function&& func) {
		BWPTree* ptr = this;
		BWPTree* prev = nullptr;
		BWPTree* tempParent = parent;
		parent = nullptr;
		while (ptr != nullptr) {
			if (ptr->lChild != nullptr && prev != ptr->lChild && prev != ptr->rChild) {//访问左节点
				prev = ptr;
				ptr = ptr->lChild;
			}
			else if (ptr->rChild != nullptr && prev != ptr->rChild) {//访问右节点
				prev = ptr;
				ptr = ptr->rChild;
			}
			else {//回溯
				if (cond(ptr)) {
					func(ptr);
				}
				prev = ptr;
				ptr = ptr->parent;
			}
		}
		parent = tempParent;
		return nullptr;
	}
};