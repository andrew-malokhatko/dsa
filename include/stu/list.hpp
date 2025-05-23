#pragma once

namespace stu
{
	template<typename T>
	class list
	{
		struct Node
		{
			T m_value{};

			Node* next{};
			Node* prev{};

			Node() = default;

			Node(T value) :
				m_value{ value }
			{
			}
		};

	public:
		struct Iterator
		{
			Node* n{};

			Iterator& operator++()
			{
				assert(n);
				n = n->next;
				return *this;
			}

			Iterator operator++(int)
			{
				assert(n);
				Iterator i{ *this };
				n = n->next;
				return i;
			}

			T& operator*()
			{
				assert(n);
				return n->m_value;
			}

			const T& operator*() const
			{
				assert(n);
				return n->m_value;
			}

			bool operator==(const Iterator& i) const
			{
				return n == i.n;
			}
		};

	public:
		list() = default;

		~list()
		{
			clear();
		}

		void clear()
		{
			Node* current = m_root;

			while (current)
			{
				Node* next = current->next;

				delete current;
				current = next;
			}

			m_root = nullptr;
		}

		void insert(T value)
		{
			Node* next = m_root;
			m_root = new Node(value);

			m_root->next = next;
			
			if (next)
			{
				next->prev = m_root;
			}
		}

		bool contains(const T& value) const
		{
			Node* current = m_root;

			while (current)
			{
				if (current->m_value == value)
				{
					return true;
				}

				current = current->next;
			}

			return false;
		}

		bool remove(const T& value)
		{
			Node* current = m_root;

			while (current)
			{
				if (current->m_value == value)
				{
					if (current->prev)
					{
						current->prev->next = current->next;
					}

					if (current->next)
					{
						current->next->prev = current->prev;
					}

					delete current;

					return true;
				}

				current = current->next;
			}

			return false;
		}

		bool empty() const
		{
			return m_root == nullptr;
		}

		const T& first() const
		{
			assert(m_root);
			return m_root->m_value;
		}

		T& first()
		{
			assert(m_root);
			return m_root->m_value;
		}

		Iterator begin()
		{
			return Iterator{ m_root };
		}

		Iterator end()
		{
			return Iterator{ nullptr };
		}

	private:
		size_t m_size{};
		Node* m_root{};
	};
}