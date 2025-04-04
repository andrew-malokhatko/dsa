#pragma once

#include <array>
#include <vector>
#include <iterator>
#include <cassert>

namespace stu
{
	//        +----100----+
	//       /             \
	//   50,51,55      150,151,155 
	//  /  |  |  \    /   |   |   \

	class btree
	{
	public:
		using value_type = int;
		using size_type = size_t;
		using max_list_size = size_t;

		static constexpr size_type max_node_size = 3;
		static constexpr size_type min_node_size = max_node_size / 2;

		static constexpr size_type max_children_size = max_node_size + 1;
		static constexpr size_type min_children_size = min_node_size + 1;
		//constexpr size_type max__size = max_node_size + 1;

	public:
		struct BNode
		{
			std::array<value_type, max_node_size + 1> keys{};
			size_t size{};
			BNode* parent{};
			std::array<BNode*, max_node_size + 2> children{};
			size_t childrenSize{};


			const bool isLeaf()
			{
				return childrenSize == 0;
			}

			const bool isRoot()
			{
				return parent == nullptr;
			}

			const bool hasParent()
			{
				return parent != nullptr;
			}

			value_type min()
			{
				return keys[0];
			}

			value_type max()
			{
				return keys[size - 1];
			}

			BNode* leftSibling()
			{
				if (!hasParent()) return nullptr;

				int childIndex = parent->findIndex(this);
				return childIndex > 0 ? parent->children[childIndex - 1] : nullptr;
			}

			BNode* rightSibling()
			{
				if (!hasParent()) return nullptr;

				int childIndex = parent->findIndex(this);
				return childIndex < parent->childrenSize - 1 ? parent->children[childIndex + 1] : nullptr;
			}

			const bool isBalanced()
			{
				if (size > max_node_size)
				{
					return false;
				}

				// not leaf node should always have size + 1 children
				if (!isLeaf() && (size + 1 != childrenSize))
				{
					return false;
				}

				if (isRoot())
				{
					return true;
				}

				return size >= min_node_size;
			}

			// index of the place where element should be inserted
			const int findInsertPos(value_type key)
			{
				int insertPos = 0;
				while (insertPos < size &&
					keys[insertPos] < key)
				{
					insertPos++;
				}
				return insertPos;
			}

			const int findInsertPos(BNode* child)
			{
				int insertPos = 0;
				while (insertPos < childrenSize &&
					keys[insertPos] < child->keys[0])
				{
					insertPos++;
				}
				return insertPos;
			}

			const int findIndex(value_type key)
			{
				for (int i = 0; i < size; i++)
				{
					if (keys[i] == key)
					{
						return i;
					}
				}

				return -1;
			}

			const int findIndex(BNode* node)
			{
				for (int i = 0; i < childrenSize; i++)
				{
					if (children[i] == node)
					{
						return i;
					}
				}

				return -1;
			}

			const bool containsKey(value_type key)
			{
				int index = findIndex(key);
				return index != -1;
			}

			void insertKey(value_type key)
			{
				int insertPos = findInsertPos(key);

				// Move all keys from insertPos to the right
				for (int i = size; i > insertPos; i--)
				{
					keys[i] = keys[i - 1];
				}

				keys[insertPos] = key;
				size++;
			}

			void removeKey(value_type key)
			{
				int removePos = findIndex(key);
				if (removePos == -1)
				{
					return;
				}

				// Move all keys 1 to the left to fill the gap
				for (int i = removePos; i < size - 1; i++)
				{
					keys[i] = keys[i + 1];
				}

				size--;
			}

			void insertChild(BNode* child)
			{
				int insertPos = findInsertPos(child);

				// Move all children from insertPos to the right
				for (int i = childrenSize; i > insertPos; i--)
				{
					children[i] = children[i - 1];
				}

				child->parent = this;

				children[insertPos] = child;
				childrenSize++;
			}

			void removeChild(BNode* child)
			{
				size_t removePos = findIndex(child);
				if (removePos == -1)
				{
					return;
				}

				// Move all keys 1 to the left to fill the gap
				for (size_t i = removePos; i < childrenSize - 1; i++)
				{
					children[i] = children[i + 1];
				}

				childrenSize--;
			}

			// recursively get all children of the node (including the node itself)
			const void getChildren(std::vector<value_type>& result)
			{
				if (isLeaf())
				{
					std::copy(keys.begin(), keys.begin() + size, std::back_inserter(result));
					return;
				}

				for (int i = 0; i < size; i++)
				{
					children[i]->getChildren(result);
					result.push_back(keys[i]);
				}

				children[childrenSize - 1]->getChildren(result);
			}

			void borrowKeyLeft(BNode* node, BNode* parent, BNode* leftSibling)
			{
				int childIndex = parent->findIndex(node);

				value_type parentToNode = parent->keys[childIndex - 1];
				value_type LeftToParent = leftSibling->max();

				leftSibling->removeKey(LeftToParent);
				parent->removeKey(parentToNode);

				parent->insertKey(LeftToParent);
				node->insertKey(parentToNode);
			}

			void borrowKeyRight(BNode* node, BNode* parent, BNode* rightSibling)
			{
				int childIndex = parent->findIndex(node);

				value_type parentToNode = parent->keys[childIndex];
				value_type rightToParent = rightSibling->min();

				rightSibling->removeKey(rightToParent);
				parent->removeKey(parentToNode);

				parent->insertKey(rightToParent);
				node->insertKey(parentToNode);
			}
		};

		void balanceInsert(BNode* node)
		{
			if (node && node->isBalanced())
			{
				return;
			}

			if (node->size > max_node_size)
			{
				split(node, node->size / 2);
				balanceInsert(node->parent);
			}
			// balance the node
		}

		void split(BNode* node, size_t index)
		{
			value_type temp = node->keys[index];
			BNode* right = new BNode();

			// transfer values from left to right
			for (int i = index + 1; i < node->size; i++)
			{
				right->insertKey(node->keys[i]);
			}
			node->size = index;

			// transfer children from left to right
			if (!node->isLeaf())
			{
				for (int i = index + 1; i < node->childrenSize; i++)
				{
					right->insertChild(node->children[i]);
				}
				node->childrenSize = index + 1;
			}

			if (node->hasParent())
			{
				BNode* parent = node->parent;
				parent->insertKey(temp);
				parent->insertChild(right);
			}
			else
			{
				// if node did not have parent before splitting
				// it was a root
				BNode* newRoot = new BNode();
				newRoot->insertKey(temp);
				newRoot->insertChild(node);
				newRoot->insertChild(right);
				root = newRoot;
			}
		}

		void merge(BNode* parent, int leftChildIndex)
		{
			assert(leftChildIndex <= parent->size - 1);

			BNode* left = parent->children[leftChildIndex];
			BNode* right = parent->children[leftChildIndex + 1];

			// transfer all of the keys to left
			for (int i = right->size - 1; i >= 0; i--)
			{
				left->insertKey(right->keys[i]);
				right->removeKey(right->keys[i]);
			}

			// insert key from the parent to keep key-children count balanced
			left->insertKey(parent->keys[leftChildIndex]);
			parent->removeKey(parent->keys[leftChildIndex]);

			// if right node is not a leaf (has kids) transfer them as well
			if (!right->isLeaf())
			{
				for (int i = right->childrenSize - 1; i >= 0; i--)
				{
					left->insertChild(right->children[i]);
					right->removeChild(right->children[i]);
				}
			}

			if (parent->size == 0)
			{
				BNode* parentsParent = parent->parent;

				if (parentsParent)
				{
					parentsParent->removeChild(parent);
					parentsParent->insertChild(left);
				}
				else
				{
					left->parent = nullptr;
					root = left;
				}

				delete parent;
			}

			assert(isSorted());

			delete right;
		}

		// Find node where key is stored, or can be stored
		BNode* findNode(value_type key)
		{
			BNode* current = root;

			while (!current->isLeaf())
			{
				if (current->containsKey(key))
				{
					return current;
				};

				size_t insertPos = current->findInsertPos(key);
				current = current->children[insertPos];
			}

			return current;
		}

		void removeFromLeaf(BNode* node, value_type key)
		{
			BNode* parent = node->parent;
			BNode* leftSibling = node->leftSibling();
			BNode* rightSibling = node->rightSibling();
			
			// Remove key first, then manage the tree structure
			node->removeKey(key);

			if (!parent || node->size > min_node_size)
			{
				return;
			}
			else
			{
				if (leftSibling && leftSibling->size > min_node_size)
				{
					node->borrowKeyLeft(node, parent, leftSibling);
				}
				else if (rightSibling && rightSibling->size > min_node_size)
				{
					node->borrowKeyRight(node, parent, rightSibling);
				}
				else
				{
					int leftIndex = leftSibling ? parent->findIndex(leftSibling) : parent->findIndex(node);
					
					merge(parent, leftIndex);
				}
			}
		}

		void removeInternal()
		{

		}

		bool isSorted(const BNode* node, const value_type* min, const value_type* max) const
		{
			// Check if the keys in the current node are in ascending order
			for (size_t i = 1; i < node->size; ++i)
			{
				if (node->keys[i - 1] >= node->keys[i])
				{
					return false;
				}
			}

			// Check if the keys are within the valid range defined by min and max
			if ((min && node->keys[0] <= *min) || (max && node->keys[node->size - 1] >= *max))
			{
				return false;
			}

			// Recursively check the child nodes
			for (size_t i = 0; i < node->childrenSize; ++i)
			{
				const value_type* newMin = (i == 0) ? min : &node->keys[i - 1];
				const value_type* newMax = (i == node->size) ? max : &node->keys[i];
				if (!isSorted(node->children[i], newMin, newMax))
				{
					return false;
				}
			}

			return true;
		}

	public:
		btree()
		{
			root = new BNode();
		}

		~btree() = default;

	public:
		const size_t size() const
		{
			return m_size;
		}

		const bool empty() const
		{
			return m_size == 0;
		}
		
		BNode* getRoot() const
		{
			return root;
		}

		const std::vector<value_type> toVector()
		{
			std::vector<value_type> keys;
			keys.reserve(m_size);

			root->getChildren(keys);

			return keys;
		}

		bool isSorted() const
		{
			if (!root)
			{
				return true;
			}

			return isSorted(root, nullptr, nullptr);
		}

		void insert(const value_type& key)
		{
			if (root == nullptr)
			{
				root = new BNode();
				root->keys[0] = key;
			}

			BNode* current = findNode(key);

			if (current->containsKey(key))
			{
				return;
			}

			m_size++;

			current->insertKey(key);
			balanceInsert(current);
		}

		const bool search(const value_type& key)
		{
			if (!root)
			{
				return false;
			}

			BNode* current = findNode(key);

			return current->findIndex(key) != -1;
		}

		void remove(const value_type& key)
		{
			if (!root)
			{
				return;
			}

			BNode* current = findNode(key);
			int index = current->findIndex(key);

			// Key is not in the tree
			if (index == -1)
			{
				return;
			}

			if (current->isLeaf())
			{
				removeFromLeaf(current, key);
			}

			m_size--;
		}


	private:
		BNode* root{};
		size_type m_size{};
	};
}