#pragma once

#include <algorithm>
#include "utils.hpp"
#include "unique_list.hpp"
#include "set.hpp"

namespace stu
{
	template<typename key_type, typename value_type>
	class hashtable
	{
		struct Bucket
		{
			struct BucketNode
			{
				key_type m_key;
				value_type m_value;

				BucketNode* next = nullptr;

				BucketNode(key_type key, value_type value, BucketNode* n = nullptr)
					: m_key(key),
					m_value(value),
					next(n)
				{
				}
			};

			// Iterator for Bucket
			struct Iterator
			{
				BucketNode* n;

				Iterator(BucketNode* node) : n(node) {}

				Iterator& operator++()
				{
					if (n) n = n->next;
					return *this;
				}

				Iterator operator++(int)
				{
					Iterator temp = *this;
					if (n) n = n->next;
					return temp;
				}

				const BucketNode& operator*() const
				{
					return *n;
				}

				BucketNode& operator*()
				{
					return *n;
				}

				bool operator==(const Iterator& other) const
				{
					return n == other.n;
				}
			};

			BucketNode* m_first = nullptr;
			size_t m_size = 0;

			~Bucket()
			{
				BucketNode* current = m_first;
				while (current != nullptr)
				{
					BucketNode* next = current->next;
					delete current;
					current = next;
				}
			}

			// ALERT! If this function returns false, then it does not take ownership of node, and it must be manually dealocated!
			//
			bool insert(BucketNode* node)
			{
				BucketNode* current = m_first;
				while (current != nullptr)
				{
					if (current->m_key == node->m_key)
					{
						//current->m_value = node->value;
						return false; // Key exists, value updated
					}
					current = current->next;
				}

				node->next = m_first;
				m_first = node;

				m_size++;
				return true; // New key inserted
			}

			bool insert(key_type key, value_type value)
			{
				BucketNode* current = m_first;
				while (current != nullptr)
				{
					if (current->m_key == key)
					{
						return false; // Key exists, value updated
					}

					current = current->next;
				}

				auto node = new BucketNode(key, value);
				node->next = m_first;
				m_first = node;

				m_size++;
				return true; // New key inserted
			}

			bool remove(key_type key)
			{
				BucketNode* current = m_first;
				BucketNode* previous = nullptr;
				while (current != nullptr)
				{
					if (current->m_key == key)
					{
						if (previous != nullptr)
						{
							previous->next = current->next;
						}
						else
						{
							m_first = current->next;
						}

						delete current;
						m_size--;
						return true;
					}
					previous = current;
					current = current->next;
				}

				return false;
			}

			bool contains(const key_type& key) const
			{
				return search(key) != end();
			}

			// Preferably should be used after contains
			const Iterator search(const key_type& key) const
			{
				for (auto it = begin(), e = end(); it != e; ++it)
				{
					if ((*it).m_key == key)
					{
						return it;
					}
				}

				return end();
			}

			Iterator search(const key_type& key)
			{
				for (auto it = begin(), e = end(); it != e; ++it)
				{
					if ((*it).m_key == key)
					{
						return it;
					}
				}

				return end();
			}

			bool empty() const
			{
				return m_size == 0;
			}

			const Iterator begin() const
			{
				return Iterator(m_first);
			}

			Iterator begin()
			{
				return Iterator(m_first);
			}

			Iterator end()
			{
				return Iterator(nullptr);
			}

			const Iterator end() const
			{
				return Iterator(nullptr);
			}
		};

	public:

		// Iterator for a hashtable
		struct Iterator
		{
			Bucket* buckets;							// Pointer to bucket array
			size_t bucket_index;						// Current bucket index
			size_t capacity;
			typename Bucket::Iterator node;				// Iterator for current bucket

			Iterator(Bucket* b, size_t index, size_t cap, typename Bucket::Iterator node_it) :
				buckets(b),
				bucket_index(index),
				capacity(cap),
				node(node_it)
			{
			}

			Iterator& operator++()
			{
				++node;
				while (bucket_index + 1 < capacity && node == buckets[bucket_index].end())
				{
					node = buckets[++bucket_index].begin(); // Move to next bucket's begin
				}
				return *this;
			}

			Iterator operator++(int)
			{
				Iterator temp = *this;
				++(*this);
				return temp;
			}

			const typename Bucket::BucketNode& operator*() const
			{
				return *node;
			}

			typename Bucket::BucketNode& operator*()
			{
				return *node;
			}

			bool operator==(const Iterator& other) const
			{
				return node == other.node && bucket_index == other.bucket_index;
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

		size_t getBucketIndex(const key_type& key) const
		{
			auto h = std::hash<key_type>{}(key); // hash(static_cast<const void*>(&key), sizeof(key_type));
			return h % m_capacity;
		}

		Bucket& getBucket(const key_type& key)
		{
			return m_buckets[getBucketIndex(key)];
		}

		const Bucket& getBucket(const key_type& key) const
		{
			return m_buckets[getBucketIndex(key)];
		}

	public:

		hashtable(size_t capacity = 16)
		{
			reserve(capacity);
		}


		~hashtable()
		{
			clear();
		}

		void clear()
		{
			delete[] m_buckets;
			m_buckets = nullptr;
			m_count = 0;
			m_capacity = 0;
		}


		bool insert(key_type key, value_type value)
		{	
			Bucket& bucket = getBucket(key);

			bool result = bucket.insert(key, value);
			if (result)
			{
				m_count++;
			}

			if (loadFactor() > maxLoadFactor)
			{
				reserve(std::max((size_t)(m_capacity * treshold), m_count));
			}

			return result;
		}

		bool erase(key_type key)
		{
			Bucket& bucket = getBucket(key);
			bool removed = bucket.remove(key);

			if (removed)
			{
				m_count--;
			}

			return removed;
		}

		const Iterator search(key_type key) const
		{
			const Bucket& bucket = getBucket(key);
			
			auto bit = bucket.search(key);
			if (bit == bucket.end())
			{
				return end();
			}

			size_t bucketIndex = getBucketIndex(key);
			return Iterator{m_buckets, bucketIndex, m_capacity, bit};
		}

		Iterator search(key_type key)
		{
			 Bucket& bucket = getBucket(key);

			auto bit = bucket.search(key);
			if (bit == bucket.end())
			{
				return end();
			}

			size_t bucketIndex = getBucketIndex(key);
			return Iterator{ m_buckets, bucketIndex, m_capacity, bit };
		}

		bool contains(key_type key) const
		{
			const Bucket& bucket = getBucket(key);

			return bucket.contains(key);
		}

		void setMaxLoadFactor(double newLoadFactor)
		{
			maxLoadFactor = newLoadFactor;
		}

		void reserve(size_t newCapacity)
		{
			newCapacity = nextPrime(newCapacity);
			size_t oldCapacity = m_capacity;
			m_capacity = newCapacity;

			Bucket* oldBuckets = m_buckets;
			m_buckets = new Bucket[newCapacity];

			for (size_t i = 0; i < oldCapacity; i++)
			{
				Bucket& oldBucket = oldBuckets[i];

				for (auto it = oldBucket.begin(); it != oldBucket.end();)
				{
					auto& node = *it;
					++it;
					node.next = nullptr;
					Bucket& newBucket = getBucket(node.m_key);
					
					bool inserted = newBucket.insert(&node);
					
					assert(inserted);
					if (inserted == false)
					{
						delete (&node);
					}
				}

				oldBucket.m_first = nullptr;
			}

			delete[] oldBuckets;
		}

		double loadFactor()
		{
			if (m_capacity == 0)
			{
				return 0;
			}

			return (double)m_count / m_capacity;
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

		bool empty()
		{
			return m_count == 0;
		}

		Iterator begin()
		{
			size_t index = 0;
			auto node = m_buckets[0].begin();
			while (index + 1 < m_capacity && node == m_buckets[index].end())
			{
				node = m_buckets[++index].begin();
			}
			return Iterator(m_buckets, index, m_capacity, node);
		}

		Iterator end()
		{
			// Return the end of the last bucket iterator
			size_t lastBucketIndex = m_capacity - 1;
			return Iterator(m_buckets, lastBucketIndex , m_capacity, m_buckets[lastBucketIndex].end());
		}

		const value_type& operator[](const key_type& key) const
		{
			return search(key);
		}

		value_type& operator[](const key_type& key)
		{
			if (!contains(key))
			{
				// insert with hashtable function to increment count
				insert(key, value_type{});
			}

			auto it = search(key);
			return (*it).m_value;
		}
	
	private:
		double maxLoadFactor = 1.0;
		const double treshold = 2.0;

		size_t m_capacity{};
		size_t m_count{};

		Bucket* m_buckets{};
	};
}