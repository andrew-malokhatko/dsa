#include <vector>
#include <iostream>
#include <unordered_set>
#include <cassert>
#include <algorithm>

namespace stu
{

	class hashmap
	{

		using key_type = int;
		using value_type = int;
		using size_type = size_t;

		static constexpr size_type startSize = 101;
		static constexpr float treshhold = 2.5f;
		static constexpr float maxLoadFactor = 0.5f;

		static bool isPrime(int n)
		{
			if (n <= 1) return false;
			for (int i = 2; i * i <= n; ++i)
				if (n % i == 0)
					return false;
			return true;
		}

		static int prime(int n)
		{
			while (!isPrime(n)) ++n;
			return n;
		}
	
	public:

		static size_type hashMain(int value)
		{
			size_type result = 0;
			int counter = 43;

			while (value)
			{
				counter = prime(counter);

				if (result % 2 == 0)
				{
					result += counter * counter * counter;
				}

				result += ((value % 17 + 89) * counter) * ((value % 17 + 89) * counter);
				result *= sqrt(value);

				counter++;
				value /= 17;
			}

			return result;
		}

		static size_type hashSecondary(size_type value)
		{
			size_type result = 0;
			int counter = 23;

			while (value)
			{
				counter = prime(counter);

				if (result % 3 == 0)
				{
					result += counter * counter + sqrt(value);
					result += value / sqrt(counter);
				}

				result += ((value % 13 + 37) * counter) * ((value % 13 + 37) * counter);
				result *= sqrt(value);

				counter++;
				value /= 13;
			}

			return result;
		}

		size_type hash(key_type key, size_type attempt) const
		{
			return (hashMain(key) + hashSecondary(key * attempt)) % m_capacity;
		}

		bool containsDuplicate() const
		{
			std::unordered_set<key_type> keys;

			for (int i = 0; i < m_capacity; i++)
			{
				if (keys.contains(map[i].key))
				{
					return false;
				}
				else
				{
					keys.insert(map[i].key);
				}
			}

			return true;
		}

		size_type findIndex(key_type key) const
		{
			size_type attempt = 0;

			while (true)
			{
				size_type currentIndex = hash(key, attempt);
				Entry current = map[currentIndex];

				if (current.key == key)
				{
					return currentIndex;
				}
				else if (current.deleted || current.collisions > 0)
				{
					attempt++;
					continue;
				}

				break;
			}

			return -1;
		}

	public:

		struct Entry
		{
			key_type key;
			value_type value;
			size_type collisions;

			// flags 
			bool valid;
			bool deleted;

			bool operator== (const Entry& other) const
			{
				return key == other.key &&
					   value == other.value;
			}
		};

	public:

		hashmap()
		{
			map = new Entry[startSize]();
			m_capacity = startSize;
		}

		hashmap(size_type size)
		{
			size = prime(size);
			map = new Entry[size]();
			m_capacity = size;
		}

		~hashmap() = default; 

		size_t size()
		{
			return m_size;
		}

		size_t capacity()
		{
			return m_capacity;
		}

		float loadFactor()
		{
			return static_cast<float>(m_size) / static_cast<float>(m_capacity);
		}

		bool empty()
		{
			return m_size == 0;
		}

		void print(bool all = false)
		{
			std::cout << "----------------------------------\n";
			for (int i = 0; i < m_capacity; i++)
			{
				if (map[i].valid == false)
				{
					if (all)
					{
						std::cout << "EMPTY\n";
					}
				}
				else
				{
					std::cout << map[i].key << " : " << map[i].value << "\n";
				}
			}
			std::cout << "----------------------------------\n";
		}

		void insert(key_type key, value_type value)
		{
			int attempt = 0;

			while (true)
			{
				size_type insertPosition = hash(key, attempt);
				
				// owerwrite value if duplicate is encountered
				if (map[insertPosition].key == key)
				{
					map[insertPosition].value = value;
					break;
				}

				// if collision happens, add collision to currentElement
				if (map[insertPosition].valid == true)
				{
					map[insertPosition].collisions++;
					attempt++;
					continue;
				}

				// loop until free position is found
				if (map[insertPosition].valid == false)
				{
					map[insertPosition] = Entry(key, value, 0, true, false);
					m_size++;

					break;
				}

				// this code should never be reached
				assert(false);
			}

			if (loadFactor() > maxLoadFactor)
			{
				reserve(m_capacity * treshhold);
			}

			assert(containsDuplicate() == false);
		}

		void reserve(size_type newSize)
		{
			if (newSize < m_capacity)
			{
				return;
			}

			hashmap newMap = hashmap(newSize);

			for (int i = 0; i < m_capacity; i++)
			{
				Entry entry = map[i];
				entry.collisions = 0;

				if (entry.valid)
				{
					newMap.insert(entry.key, entry.value);
				}
			}

			assert(newMap.m_size == m_size);

			delete[] map;

			m_capacity = newMap.m_capacity;
			map = newMap.map;

			assert(containsDuplicate() == false);
		}

		void remove(key_type key)
		{
			size_type index = findIndex(key);

			if (index != -1)
			{
				Entry& current = map[index];

				current.key = 0;
				current.value = 0;
				current.collisions = 0;
				current.valid = false;
				current.deleted = true;
			}

			assert(containsDuplicate() == false);
		}

		bool containsKey(key_type key)
		{
			size_type index = findIndex(key);
			return index != -1;
		}

		value_type search(key_type key)
		{
			size_type index = findIndex(key);
			if (index != -1)
			{
				return map[index].value;
			}

			return 0;
		}

		// Returns a vector sorted by keys
		std::vector<Entry> toVector() const
		{
			std::vector<Entry> result;
			result.reserve(m_size);

			for (size_type i = 0; i < m_capacity; ++i) 
			{
				Entry& entry = map[i];
				if (entry.valid && !entry.deleted)
				{
					result.push_back(entry);
				}
			}

			std::sort(result.begin(), result.end(), [](const Entry& a, const Entry& b)
				{
					return a.key < b.key;
				});

			return result;
		}

	private:
		size_type m_size{};
		size_type m_capacity{};
		Entry* map;
	};
}