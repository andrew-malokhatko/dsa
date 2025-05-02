#pragma once

#include "utils.hpp"
#include "unique_list.hpp"

namespace stu
{
	template <typename first_t, typename second_t>
	struct pair
	{
		first_t first;
		second_t second;
	};
	
	template<typename key_type, typename value_type, typename list_type = stu::unique_list<stu::pair<key_type, value_type>>>
	class hashtable
	{
		struct Bucket
		{
			list_type values{};

			bool insert(key_type key, value_type value)
			{
				//return values.insert({ key, value });
				return {};
			}

			bool remove(key_type key)
			{
				return values.remove(key);
			}

			const value_type& search(key_type key) const
			{
				return values.search(key);
			}

			bool empty() const
			{
				return values.empty();
			}

			const value_type& first() const
			{
				return values.first();
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
			//const Bucket& bucket = getBucket(key);

			//return bucket.search(key) != value_type{};
			// TODO
			return false;
		}

		void setMaxLoadFactor(double newLoadFactor)
		{
			maxLoadFactor = newLoadFactor;
		}

		void reserve(size_t newCapacity)
		{
			//newCapacity = nextPrime(newCapacity);

			//Bucket* oldBuckets = m_buckets;
			//m_buckets = new Bucket[newCapacity];

			//size_t oldCapacity = m_capacity;
			//m_capacity = newCapacity;

			//for (size_t i = 0; i < oldCapacity; i++)
			//{
			//	Bucket& oldBucket = oldBuckets[i];
			//	Node* current = oldBucket.root;

			//	while (current)
			//	{
			//		Node* next = current->next; // remember next before moving

			//		Bucket newBucket = getBucket(current->m_key);
			//		newBucket.insert(current);

			//		current = next;
			//	}

			//	oldBucket.root = nullptr;
			//}

			//delete[] oldBuckets;
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