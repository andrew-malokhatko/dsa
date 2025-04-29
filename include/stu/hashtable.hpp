#include "stu/utils.hpp"

namespace stu
{
	
	template<typename key_type, typename value_type>
	class hashtable
	{

		struct Bucket
		{
			struct Node
			{
				key_type m_key{};
				value_type m_value{};

				Node* next{};
				Node* prev{};
			};

			size_t m_size{};
			Node* root{};

			~Bucket()
			{
				Node* current = root;

				while (current)
				{
					Node* next = current->next;

					delete current;
					current = next;
				}
			}


			bool insert(Node* node)
			{
				Node* current = root;

				if (!current)
				{
					root = node;
					m_size++;
					return true;
				}

				while (current->next)
				{
					if (current->m_key == node->m_key)
					{
						return false;
					}
				}

				current->next = node;
				node->prev = current;

				m_size++;
				return true;
			}

			bool insert(key_type key, value_type value)
			{
				return insert(new Node(key, value));
			}

			bool remove(key_type key)
			{
				Node* current = root;

				while (current)
				{
					if (current->m_key == key)
					{
						if (current->prev)
						{
							assert(current->prev->next == current);
							current->prev->next = current->next;
						}

						if (current->next)
						{
							current->next->prev = current->prev;
						}

						if (current == root)
						{
							root = current->next;
						}

						delete current;

						m_size--;
						return true;
					}
				}

				return false;
			}

			const value_type& search(key_type key)
			{
				Node* current = root;

				while (current)
				{
					if (current->m_key == key)
					{
						return current->m_value;
					}
				}

				return nullptr;
			}

			const bool empty() const
			{
				return size
			}

			const Node* first() const
			{
				return root;
			}
		};

	private:
		size_t hash(const void* data, size_t byteSize)
		{
			size_t hash = 0;
			const char* prt = (const char*)data;

			while (byteSize--)
			{
				hash += (hash << 5) + *prt;
				prt++;
			}

			return hash + 89;
		}

	public:

		hashtable(size_t capacity = 500)
		{
			reserve(capacity);
		}

		bool insert(key_type key, value_type value)
		{
			size_t index = hash(key, sizeof(key_type)) % m_capacity;
			
			Bucket bucket = buckets[index];
			bool result = bucket.insert(key, value);

			if (loadFactor() > maxLoadFactor)
			{
				reserve(m_capacity * treshold);
			}

			return result;
		}

		bool remove(key_type key)
		{
			size_t index = hash(key, sizeof(key_type)) % m_capacity;

			Bucket bucket = buckets[index];

			return bucket.remove(key);
		}

		const value_type& search(key_type key)
		{
			size_t index = hash(key, sizeof(key_type)) % m_capacity;

			Bucket bucket = buckets[index];

			return bucket.search(key);
		}

		void setMaxLoadFactor(double newLoadFactor)
		{
			maxLoadFactor = newLoadFactor;
		}

		void reserve(size_t newCapacity)
		{
			newCapacity = nextPrime(newCapacity);

			Bucket* oldBuckets = buckets;
			buckets = new Bucket[newCapacity];

			for (Bucket bucket : oldBuckets)
			{
				if (!bucket.empty())
				{
					size_t newPlace = hash(bucket.first().m_key, sizeof(key_type)) % newCapacity;
					buckets[newPlace] = bucket;
				}
			}

			m_capacity = newCapacity;
		}

		double loadFactor()
		{
			return m_count / m_capacity;
		}

		~hashtable()
		{
			delete[] buckets;
		}


	
	private:
		double maxLoadFactor = 1.0;
		double treshold;

		size_t m_capacity{};
		size_t m_count{};

		Bucket* buckets{};
	};
}