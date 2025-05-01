#pragma once

#include "utils.hpp"

namespace stu
{
	
	template<typename key_type, typename value_type>
	class hashtable
	{

	public:

		struct Node
		{
			key_type m_key{};
			value_type m_value{};

			Node* next{};
			Node* prev{};

			Node() = default;

			Node(key_type key, value_type value) :
				m_key{ key },
				m_value{ value }
			{
			}

			bool operator== (const Node& other) const
			{
				return m_key == other.m_key &&
					m_value == other.m_value;
			}
		};

		struct Bucket
		{
			size_t size{};
			Node* root{};

			Bucket() = default;

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

				// if node is reinserted
				node->next = nullptr;
				node->prev = nullptr;

				if (!current)
				{
					root = node;
					size++;
					return true;
				}

				while (current->next)
				{
					if (current->m_key == node->m_key)
					{
						return false;
					}

					current = current->next;
				}
				
				current->next = node;
				node->prev = current;

				size++;
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

						size--;
						return true;
					}

					current = current->next;
				}

				return false;
			}

			const value_type& search(key_type key) const
			{
				Node* current = root;

				while (current)
				{
					if (current->m_key == key)
					{
						return current->m_value;
					}

					current = current->next;
				}

				return value_type{};
			}

			const bool empty() const
			{
				return size;
			}

			const Node* first() const
			{
				return root;
			}
		};

	private:
		size_t hash(const void* data, size_t byteSize) const
		{
			size_t hash = 0;
			const char* ptr = (const char*)data;

			while (byteSize--)
			{
				hash += (hash << 5) + *ptr;
				ptr++;
			}

			return hash + 89;
		}

		size_t getHash(const key_type& key) const
		{
			return hash(static_cast<const void*>(&key), sizeof(key_type)) % m_capacity;
		}

		Bucket& getBucket(key_type& key)
		{
			size_t index = getHash(key);

			return m_buckets[index];
		}

		const Bucket& getBucket(const key_type& key) const
		{
			return m_buckets[getHash(key) % m_capacity];
		}

	public:

		hashtable(size_t capacity = 500)
		{
			reserve(capacity);
		}


		~hashtable()
		{
			delete[] m_buckets;
		}


		bool insert(key_type key, value_type value)
		{	
			Bucket& bucket = getBucket(key);

			bool result = bucket.insert(key, value);

			if (loadFactor() > maxLoadFactor)
			{
				reserve(m_capacity * treshold);
			}

			return result;
		}

		bool remove(key_type key)
		{
			Bucket& bucket = getBucket(key);

			return bucket.remove(key);
		}

		const value_type& search(key_type key) const
		{
			const Bucket& bucket = getBucket(key);
			
			assert(contains(key));

			return bucket.search(key);
		}

		bool contains(key_type key) const
		{
			const Bucket& bucket = getBucket(key);

			return bucket.search(key) != value_type{};
		}

		void setMaxLoadFactor(double newLoadFactor)
		{
			maxLoadFactor = newLoadFactor;
		}

		void reserve(size_t newCapacity)
		{
			newCapacity = nextPrime(newCapacity);

			Bucket* oldBuckets = m_buckets;
			m_buckets = new Bucket[newCapacity];

			size_t oldCapacity = m_capacity;
			m_capacity = newCapacity;

			for (size_t i = 0; i < oldCapacity; i++)
			{
				Bucket& oldBucket = oldBuckets[i];
				Node* current = oldBucket.root;

				while (current)
				{
					Node* next = current->next; // remember next before moving

					Bucket newBucket = getBucket(current->m_key);
					newBucket.insert(current);

					current = next;
				}

				oldBucket.root = nullptr;
			}

			delete[] oldBuckets;
		}

		double loadFactor()
		{
			return m_count / m_capacity;
		}

		Bucket* getBuckets() const
		{
			return m_buckets;
		}

		size_t getCapacity() const
		{
			return m_capacity;
		}

		size_t getCount() const
		{
			return m_count;
		}

	
	private:
		double maxLoadFactor = 1.0;
		double treshold;

		size_t m_capacity{};
		size_t m_count{};

		Bucket* m_buckets{};
	};
}