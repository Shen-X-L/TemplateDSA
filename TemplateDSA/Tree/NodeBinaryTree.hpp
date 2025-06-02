#include<stack>
template<typename T>
class NodeBinaryTree {
private:
	// 节点类定义
	template<typename U>
	class _Node {
	public:
		using node_type = _Node<U>;
		friend class NodeBinaryTree;
	private:
		U _data;
		node_type* _left = nullptr;
		node_type* _right = nullptr;
	public:
		explicit _Node(const U& value,node_type* left = nullptr,node_type* right = nullptr)
			:_data(value), _left(left), _right(right) {}
		explicit _Node(U&& value, node_type* left = nullptr, node_type* right = nullptr)
			:_data(std::move(value)), _left(left), _right(right) {
		}
	};
public:
	using value_type = T;
	using pointer = T*;
	using reference = T&;
	using const_reference = const T&;
	using node_type = _Node<T>;
	using NBTree = NodeBinaryTree<T>;
private:
	node_type* _root = nullptr;
public:
	NodeBinaryTree() noexcept = default;

	NodeBinaryTree(NBTree&& other) noexcept :_root(other._root) { other._root = nullptr; };

	// 主拷贝构造函数
	NodeBinaryTree(const NodeBinaryTree& other)
		: _root(_iterativeCopy<std::stack>(other._root)) {}

	// 模板化替代方案
	template <template <typename> class StackType>
	NodeBinaryTree(const NodeBinaryTree& other, StackType<std::pair<Node*, Node**>>* = nullptr)
		: _root(_iterativeCopy<StackType>(other._root)) {}

	explicit NodeBinaryTree(node_type*&& root_node) noexcept
		: _root(root_node) {
		root_node = nullptr;
	}

	template <template<typename> class StackType>
	explicit NodeBinaryTree(const node_type* root_node)
		: _root(_iterativeCopy<StackType>(root_node)) {
	}

private:

	// 递归拷贝辅助函数
	node_type* _recursiveCopy(node_type* node) const {
		if (!node) return nullptr;
		return new node_type(node->data,
						recursiveCopy(node->left),
						recursiveCopy(node->right));
	}

	// 迭代拷贝辅助函数
	template <template <typename> class StackType = std::stack>
	node_type* _iterativeCopy(node_type* srcRoot) const {
		if (!srcRoot) return nullptr;

		StackType<std::pair<node_type*, node_type**>> stack;
		node_type* newRoot = new node_type(srcRoot->_data);
		stack.push({ srcRoot, &newRoot });//入栈

		while (!stack.empty()) {
			//获取并出栈
			auto [src, dst] = stack.top();
			stack.pop();
			if (src->_left) {
				*dst = new node_type(src->_left->_data);
				stack.push({ src->_left, &((*dst)->_left) });
			}
			if (src->_right) {
				*dst = new node_type(src->_right->_data);
				stack.push({ src->_right, &((*dst)->_right) });
			}
		}
		return newRoot;
	}
public:
	static reference get_data(node_type* node) { return node->_data; }
	static const_reference get_data(node_type* node) const { return node->_data; }
	static node_type* get_left_ptr(node_type* node) { return node->_left; }
	static const node_type* get_left_ptr(node_type* node) const { return node->_left; }
	static node_type* get_right_ptr(node_type* node) { return node->_right; }
	static const node_type* get_right_ptr(node_type* node) const { return node->_right; }
	static NodeBinaryTree cut_left(node_type* node) {
		if (!node) return NodeBinaryTree(nullptr);

		node_type* ptr = node->_left;
		node->_left = nullptr;

		return NodeBinaryTree(std::move(ptr));
	}
	static NodeBinaryTree cut_right(node_type* node) {
		if (!node) return NodeBinaryTree(nullptr);

		node_type* ptr = node->_right;
		node->_right = nullptr;

		return NodeBinaryTree(std::move(ptr));
	}

};