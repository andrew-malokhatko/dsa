#pragma once

#include <array>
#include <vector>
#include <iterator>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>

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
			std::array<value_type, max_node_size + 1> keys_{};
			size_t size{};
			BNode* parent{};
			std::array<BNode*, max_children_size + 1> children_{};
			size_t childrenSize{};

			value_type& keys(size_t index)
			{
				assert(index < keys_.size());
				return keys_[index];
			}

			const value_type& keys(size_t index) const
			{
				assert(index < keys_.size());
				return keys_[index];
			}

			using BNodePtr = BNode*;
			const BNodePtr& children(size_t index) const
			{
				assert(index < children_.size());
				return children_[index];
			}

			BNodePtr& children(size_t index)
			{
				assert(index < children_.size());
				return children_[index];
			}

			bool isLeaf() const
			{
				return childrenSize == 0;
			}

			bool isRoot() const
			{
				return parent == nullptr;
			}
			
			bool hasParent() const
			{
				return parent != nullptr;
			}

			value_type min() const
			{
				return keys(0);
			}

			value_type max() const
			{
				return keys(size - 1);
			}

			BNode* leftSibling() const
			{
				if (!hasParent()) return nullptr;

				int childIndex = parent->findIndex(this);
				return childIndex > 0 ? parent->children(childIndex - 1) : nullptr;
			}

			BNode* rightSibling() const
			{
				if (!hasParent()) return nullptr;

				int childIndex = parent->findIndex(this);
				assert(childIndex != -1);
				return childIndex < parent->childrenSize - 1 ? parent->children(childIndex + 1) : nullptr;
			}

			bool isBalanced() const
			{

				if (size > max_node_size)
				{
					return false;
				}

				if (isRoot())
				{
					return true;
				}

				// not leaf node should always have size + 1 children
				if (!isLeaf() && (size + 1 != childrenSize))
				{
					return false;
				}

				return size >= min_node_size;
			}

			// index of the place where element should be inserted
			int findInsertPos(value_type key) const
			{
				assert(size <= max_node_size);

				int insertPos = 0;
				while (insertPos < size &&
					keys(insertPos) < key)
				{
					insertPos++;
				}
				return insertPos;
			}

			int findInsertPos(BNode* child) const
			{
				int insertPos = 0;
				while (insertPos < childrenSize &&
					//keys[insertPos] < child->keys[0])
					this->children(insertPos)->keys(0) < child->keys(0))
				{
					insertPos++;
				}
				return insertPos;
			}

			int findIndex(value_type key) const
			{
				for (int i = 0; i < size; i++)
				{
					if (keys(i) == key)
					{
						return i;
					}
				}

				return -1;
			}

			int findIndex(const BNode* node) const
			{
				for (int i = 0; i < childrenSize; i++)
				{
					if (children(i) == node)
					{
						return i;
					}
				}

				return -1;
			}

			bool containsKey(value_type key) const
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
					keys(i) = keys(i - 1);
				}

				keys(insertPos) = key;
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
					keys(i) = keys(i + 1);
				}

				size--;
			}

			void insertChild(BNode* child)
			{
				int insertPos = findInsertPos(child);

				// Move all children from insertPos to the right
				for (int i = childrenSize; i > insertPos; i--)
				{
					children(i) = children(i - 1);
				}

				child->parent = this;

				children(insertPos) = child;
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
					children(i) = children(i + 1);
				}
				
				children(childrenSize - 1) = nullptr;
				childrenSize--;
			}

			// recursively get all children of the node (including the node itself)
			void getChildren(std::vector<value_type>& result) const
			{
				if (isLeaf())
				{
					if (size >= keys_.size())
					{
						std::cout << "ASSERT: getChildren, size " << size << "\n";
						assert(size < keys_.size());
					}
					std::copy(keys_.begin(), keys_.begin() + size, std::back_inserter(result));
					return;
				}

				for (int i = 0; i < size; i++)
				{
					children(i)->getChildren(result);
					result.push_back(keys(i));
				}

				children(childrenSize - 1)->getChildren(result);
			}

			void borrowLeft(BNode* node, BNode* parent, BNode* leftSibling)
			{
				int childIndex = parent->findIndex(node);

				value_type parentToNode = parent->keys(childIndex - 1);
				value_type LeftToParent = leftSibling->max();

				leftSibling->removeKey(LeftToParent);
				parent->removeKey(parentToNode);

				parent->insertKey(LeftToParent);
				node->insertKey(parentToNode);

				// also borrow a child
				if (!node->isLeaf())
				{
					BNode* maxChild = leftSibling->children(leftSibling->childrenSize - 1);
					leftSibling->removeChild(maxChild);
					node->insertChild(maxChild);
				}
			}

			void borrowRight(BNode* node, BNode* parent, BNode* rightSibling)
			{
				int childIndex = parent->findIndex(node);

				value_type parentToNode = parent->keys(childIndex);
				value_type rightToParent = rightSibling->min();

				rightSibling->removeKey(rightToParent);
				parent->removeKey(parentToNode);

				parent->insertKey(rightToParent);
				node->insertKey(parentToNode);

				// also borrow a child
				if (!node->isLeaf())
				{
					BNode* minChild = rightSibling->children(0);
					rightSibling->removeChild(minChild);
					node->insertChild(minChild);
				}
			}

			BNode* getPredecessor(int index) const
			{
				BNode* current = children(index);
				while (!current->isLeaf())
				{
					current = current->children(current->childrenSize - 1);
				}

				return current;
			}

			BNode* getSuccessor(int index) const
			{
				BNode* current = children(index + 1);
				while (!current->isLeaf())
				{
					current = current->children(0);
				}

				return current;
			}

			bool hasPredecessor(int index)
			{
				BNode* current = children(index);
				while (!current->isLeaf())
				{
					if (current->size > min_node_size)
					{
						return true;
					}
					current = current->children(current->childrenSize - 1);
				}

				return current->size > min_node_size;
			}

			bool hasSuccessor(int index)
			{
				BNode* current = children(index + 1);
				while (!current->isLeaf())
				{
					if (current->size > min_node_size)
					{
						return true;
					}
					current = current->children(0);
				}

				return current->size > min_node_size;
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

		void balanceRemove(BNode* node)
		{
			assert(node);

			BNode* parent = node->parent;
			BNode* leftSibling = node->leftSibling();
			BNode* rightSibling = node->rightSibling();

			if (node->isBalanced())
			{
				return;
			}

			// If the deficient node's left sibling exists and has more than the minimum number of elements, then rotate right
			if (leftSibling && leftSibling->size > min_node_size)
			{
				node->borrowLeft(node, parent, leftSibling);
			}
			// If the deficient node's right sibling exists and has more than the minimum number of elements, then rotate left
			else if (rightSibling && rightSibling->size > min_node_size)
			{
				node->borrowRight(node, parent, rightSibling);
			}
			// Otherwise, if both immediate siblings have only the minimum number of elements, then merge with a sibling
			// sandwiching their separator taken off from their parent
			else
			{
				if (parent)
				{
					int leftIndex = leftSibling ? parent->findIndex(leftSibling) : parent->findIndex(node);
					merge(parent, leftIndex);
				}
				else
				{
					std::cout << "Maange\n";
				}
			}

			if (parent && !parent->isBalanced())
			{
				balanceRemove(parent);
			}
		}

		void split(BNode* node, size_t index)
		{
			value_type temp = node->keys(index);
			BNode* right = new BNode();

			// transfer values from left to right
			for (int i = index + 1; i < node->size; i++)
			{
				right->insertKey(node->keys(i));
			}
			node->size = index;

			// transfer children from left to right
			if (!node->isLeaf())
			{
				for (int i = index + 1; i < node->childrenSize; i++)
				{
					right->insertChild(node->children(i));
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

			BNode* left = parent->children(leftChildIndex);
			BNode* right = parent->children(leftChildIndex + 1);

			// transfer all of the keys to left
			for (int i = right->size - 1; i >= 0; i--)
			{
				left->insertKey(right->keys(i));
				right->removeKey(right->keys(i));
			}

			// insert key from the parent to keep key-children count balanced
			left->insertKey(parent->keys(leftChildIndex));
			parent->removeKey(parent->keys(leftChildIndex));
			parent->removeChild(right);

			// if right node is not a leaf (has kids) transfer them as well
			if (!right->isLeaf())
			{
				for (int i = right->childrenSize - 1; i >= 0; i--)
				{
					left->insertChild(right->children(i));
					right->removeChild(right->children(i));
				}
			}

			//if (parent->size == 0)
			//{
			//	BNode* parentsParent = parent->parent;

			//	if (parentsParent)
			//	{
			//		parentsParent->removeChild(parent);
			//		parentsParent->insertChild(left);
			//	}
			//	else
			//	{
			//		left->parent = nullptr;
			//		root = left;
			//	}

			//	delete parent;
			//}
			delete right;
		}

		void mergeNoParent(BNode* parent, int leftChildIndex)
		{
			BNode* left = parent->children(leftChildIndex);
			BNode* right = parent->children(leftChildIndex + 1);

			// transfer all of the keys to left
			for (int i = right->size - 1; i >= 0; i--)
			{
				left->insertKey(right->keys(i));
				right->removeKey(right->keys(i));
			}

			// insert key from the parent to keep key-children count balanced
			parent->removeChild(right);

			// if right node is not a leaf (has kids) transfer them as well
			if (!right->isLeaf())
			{
				for (int i = right->childrenSize - 1; i >= 0; i--)
				{
					assert(i <= max_children_size);

					left->insertChild(right->children(i));
					right->removeChild(right->children(i));
				}

				// merge predecessor child with successor child
				// they can be merged because earlier we have tried borrowing
				// from both, but neither was possible
				mergeNoParent(left, left->childrenSize / 2 - 1);
			}

			delete right;
		}

		
		// Find node where key is stored, or can be stored
		const BNode* findNode(value_type key) const
		{
			BNode* current = root;

			while (!current->isLeaf())
			{
				if (current->containsKey(key))
				{
					return current;
				};

				size_t insertPos = current->findInsertPos(key);
				current = current->children(insertPos);
			}

			return current;
		}

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
				current = current->children(insertPos);
			}

			return current;
		}

		void removeFromLeaf(BNode* node, value_type key)
		{
			node->removeKey(key);
			balanceRemove(node);
			//
			//if (!isSorted())
			//{
			//	assert(isSorted());
			//}
		}

		void removeInternal(BNode* node, value_type key)
		{
			//if (key == 55)
			//{
			//	std::cout << "BEFORE KEY 55: \n";
			//	print(__LINE__);
			//}

			int index = node->findIndex(key);

			BNode* leftChild = node->children(index);
			BNode* rightChild = node->children(index + 1);
			BNode* predecessor = node->getPredecessor(index);
			BNode* successor = node->getSuccessor(index);

			if (node->hasPredecessor(index) || leftChild->size > min_node_size)
			{
				value_type predecessorKey = predecessor->max();

				node->removeKey(key);
				node->insertKey(predecessorKey);
				removeFromLeaf(predecessor, predecessorKey);

				assert(isSorted());
			}
			else if (node->hasSuccessor(index) || rightChild->size > min_node_size)
			{
				value_type successorKey = successor->min();
				
				node->removeKey(key);
				node->insertKey(successorKey);
				removeFromLeaf(successor, successorKey);

				assert(isSorted());
			}
			else if (node->size > min_node_size)
			{
				BNode* predecessor = node->getPredecessor(index);
				value_type predecessorKey = predecessor->max();

				node->removeKey(key);
				node->insertKey(predecessorKey);
				removeFromLeaf(predecessor, predecessorKey);

				assert(isSorted());
			}
			else
			{
				// debugging
				/*if (key == 63)
				{
					print(__LINE__);
				}*/
				mergeNoParent(node, index);

				node->removeKey(key);
				balanceRemove(node);

				assert(isSorted());
				/*if (!isSorted())
				{
					print(__LINE__);
					assert(isSorted());
				}*/
			}
			/*
			else
			{
				// try borrowing the key
				BNode* parent = node->parent;
				BNode* leftSibling = node->leftSibling();
				BNode* rightSibling = node->rightSibling();

				if (leftSibling && leftSibling->size > min_node_size)
				{
					node->removeKey(key);
					mergeNoParent(parent, index);
					node->borrowLeft(node, parent, leftSibling);
					
					assert(isSorted());
				}
				else if (rightSibling && rightSibling->size > min_node_size)
				{
					node->removeKey(key);
					mergeNoParent(node, index);
					node->borrowRight(node, parent, rightSibling);

					assert(isSorted());
				}
				else
				{
					if (leftSibling)
					{
						merge(parent, parent->findIndex(leftSibling));
					}
					else
					{
						merge(parent, parent->findIndex(node));
					}

					assert(isSorted());
				}
			}*/
		}

		bool isSorted(const BNode* node, const value_type* min, const value_type* max, int depth, int& leafDepth) const
		{
			// Check if the keys in the current node are in ascending order
			for (size_t i = 1; i < node->size; ++i)
			{
				if (node->keys(i - 1) >= node->keys(i))
				{
					return false;
				}
			}

			if (node->size >= node->keys_.size())
			{

				std::cout << "\n\n\n";
				printBTree(node);
				std::cout << "\n\n\n";

				//assert(node->size < node->keys_.size());
				return false;
			}

			// Check if the keys are within the valid range defined by min and max
			if ((min && node->keys(0) <= *min) || (max && node->keys(node->size - 1) >= *max))
			{
				return false;
			}

			if (node->isLeaf())
			{
				if (leafDepth == -1)
				{
					leafDepth = depth;
				}
				else if (depth != leafDepth)
				{
					return false;
				}
			}

			// Recursively check the child nodes
			for (size_t i = 0; i < node->childrenSize; ++i)
			{
				const value_type* newMin = (i == 0) ? min : &node->keys(i - 1);
				const value_type* newMax = (i == node->size) ? max : &node->keys(i);
				if (!isSorted(node->children(i), newMin, newMax, depth + 1, leafDepth))
				{
					return false;
				}
			}

			if (!node->isLeaf())
			{
				if (node->size == node->childrenSize)
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

			int leafDepth = -1;
			return isSorted(root, nullptr, nullptr, 0, leafDepth);
		}

		void insert(const value_type& key)
		{
			if (root == nullptr)
			{
				root = new BNode();
				root->keys(0) = key;
			}

			BNode* current = findNode(key);

			if (current->containsKey(key))
			{
				return;
			}

			m_size++;

			current->insertKey(key);
			balanceInsert(current);

			assert(isSorted());
		}

		bool search(const value_type& key) const
		{
			if (!root)
			{
				return false;
			}

			const BNode* current = findNode(key);

			assert(isSorted());

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
			else
			{
				removeInternal(current, key);
			}

			if (root->size == 0)
			{
				if (root->childrenSize == 1)
				{
					root = root->children(0);

					delete root->parent;
					root->parent = nullptr;
				}
			}
			
			/*if (!isSorted())
			{
				print(__LINE__);
				assert(isSorted());
			}*/

			m_size--;
		}

		void print(int line) const
		{
			std::cout << "--------------" << "   " << line << "   " << "---------------------\n"; 
			printBTree(root);
			std::cout << "-----------------------------------\n";
		}

		void printBTree(const BNode* node, int depth = 0, int indent = 4) const
		{
			//const int mid = 28;
			if (node->isLeaf() == false && node->size + 1 != node->childrenSize)
			{
				std::cout << "INVARIANT IS VIOLATED, size " << node->size << ", childrenSize " << node->childrenSize << "\n";
			}

			if (!node) return;

			auto indentFunc = [](int depth) 
				{
					for (int i = 0; i < depth; i++)
					{
						std::cout << "\t";
					}
				};

			// isLeaf
			//if (node->isLeaf() == true)
			//{
			//	indentFunc(depth);
			//	std::cout << "LEAF" << "\n";
			//}

			// Print keys
			indentFunc(depth);
			std::cout << "KEYS: ";
			for (size_t i = 0; i < node->size; i++)
			{
				std::cout << node->keys(i) << ", ";
			}
			std::cout << "\n";
			
			for (size_t i = 0; i < node->childrenSize; i++)
			{
				indentFunc(depth);
				if (i < node->size)
				{
					std::cout << "KEY " << node->keys(i) << ":\n";
				}
				else
				{
					std::cout << "LAST KEY:\n";
				}
				printBTree(node->children(i), depth + 1);
			}
		}

		private:
			BNode* root{};
			size_type m_size{};
	};
}