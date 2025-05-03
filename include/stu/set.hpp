#pragma once

namespace stu
{
	template<typename value_type, class Compare = std::less<value_type>>
	class set
	{
		struct Node
		{
			value_type value;

			Node* left{};
			Node* right{};
			Node* parent{};

			Node(value_type value) :
				value{ value }
			{
			}

			Node(value_type value, Node* parent) :
				value{ value },
				parent{ parent }
			{
			}

			~Node()
			{
				delete left;
				delete right;
				delete parent;
			}

			bool isLeaf()
			{
				return left == nullptr && right == nullptr;
			}

			bool isRoot()
			{
				return parent == nullptr;
			}

			Node* getPredecessor() const
			{
				Node* current = left;
				while (current && current->right)
				{
					current = current->right;
				}
				return current;
			}

			Node* getSuccessor() const
			{
				Node* current = right;
				while (current && current->left)
				{
					current = current->left;
				}
				return current;
			}
		};

	public:
		set()
		{

		}

		~set()
		{
			clear();
		}

		void clear()
		{
			clear(m_root);
		}

		bool insert(const value_type& value)
		{
			if (!m_root)
			{
				m_size++;
				m_root = new Node(value);
				return true;
			}

			Node* current = m_root;

			while (current)
			{
				if (comp(value, current->value))
				{
					if (!current->left)
					{
						current->left = new Node(value, current);
						m_size++;
						return true;
					}

					current = current->left;
					continue;
				}

				if (comp(current->value, value))
				{
					if (!current->right)
					{
						current->right = new Node(value, current);
						m_size++;
						return true;
					}

					current = current->right;
					continue;
				}

				return false;
			}

			return false;
		}

		bool remove(const value_type& value)
		{
			Node* current = m_root;

			while (current)
			{
				if (comp(value, current->value))
				{
					current = current->left;
					continue;
				}

				if (comp(current->value, value))
				{
					current = current->right;
					continue;
				}
				
				removeNode(current);
				m_size--;
				return true;
			}

			return false;
		}

		bool contains(const value_type& value) const
		{
			if (!m_root)
			{
				return false;
			}

			Node* current = m_root;
			while (current)
			{
				if (comp(value, current->value))
				{
					current = current->left;
					continue;
				}

				if (comp(current->value, value))
				{
					current = current->right;
					continue;
				}

				return true;
			}

			return false;
		}

		size_t size() const
		{
			return m_size;
		}

	private:

		void clear(Node* node)
		{
			if (!node)
			{
				return;
			}

			clear(node->left);
			clear(node->right);

			node->left = nullptr;
			node->right = nullptr;
			node->parent = nullptr;

			delete node;
		}

		void removeNode(Node* node)
		{
			if (node->left == nullptr)
			{
				// Case 1: No left child, replace with right child (could be nullptr)
				Node* child = node->right;
				if (node->parent)
				{
					if (node->parent->left == node)
					{
						node->parent->left = child;
					}
					else
					{
						node->parent->right = child;
					}
				}
				else
				{
					m_root = child;
				}

				if (child)
				{
					child->parent = node->parent;
				}

				node->right = nullptr;
				node->left = nullptr;
				node->parent = nullptr;
				delete node;
			}
			else
			{
				// Case 2: Has left child, find predecessor and replace value
				Node* pred = node->left;
				while (pred->right)
				{
					pred = pred->right;
				}
				node->value = pred->value;

				removeNode(pred); // Recursively delete predecessor (has no right child)
			}
		}

	private:
		size_t m_size{};
		Node* m_root{};

		Compare comp{};
	};
}