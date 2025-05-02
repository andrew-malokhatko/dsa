#pragma once

namespace stu
{
	template<typename T>
	class unique_list
	{
	private:
		struct Node
		{
			T m_value{};

			Node* next{};
			Node* prev{};

			Node() = default;

			Node(T value):
				m_value{ value }
			{
			}
		};

	public:

		unique_list() = default;

		~unique_list()
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

		template<typename T>
		bool insert(T&& value)
		{
			Node* current = m_root;

			while (current->next)
			{
				if (current->m_value == value)
				{
					return false;
				}

				current = current->next;
			}

			current->next = new Node(std::forward<T>(value));
			current->next->prev = current->next;

			return true;
		}

		bool contains(T value) const
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

		bool remove(T value)
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


	private:
		size_t m_size{};
		Node* m_root{};
	};
}