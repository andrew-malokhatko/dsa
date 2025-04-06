#include <vector>

namespace stu
{

	class hashmap
	{

		using key_type = int;
		using value_type = int;
		using size_type = size_t;

		static constexpr size_type startSize = 101;
		static constexpr float treshhold = 2.5f;

		bool isPrime(int n)
		{
			if (n <= 1) return false;
			for (int i = 2; i * i <= n; ++i)
				if (n % i == 0)
					return false;
			return true;
		}

		int prime(int n)
		{
			while (!isPrime(n)) ++n;
			return n;
		}

		size_type hashMain(char*)
		{

		}

		size_type hashSecondary(size_type number)
		{

		}

	public:

		struct Entry
		{
			key_type key;
			value_type value;

			// flags 
			bool valid;
			bool deleted;
		};

	public:

		hashmap()
		{
			map = new Entry[startSize]();
			m_size = startSize;
		}

		hashmap(size_type size)
		{
			size = prime(size);
			map = new Entry[size]();
			m_size = size;
		}

		~hashmap() = default;

		void insert(key_type key, value_type value)
		{

		}

		void reserve(size_type newSize)
		{
			if (newSize < m_size)
			{
				return;
			}

			hashmap newMap = hashmap(newSize);

			for (int i = 0; i < m_size; i++)
			{
				Entry entry = map[i];

				if (entry.valid)
				{
					newMap.insert(entry.key, entry.value);
				}
			}

			delete[] map;

			m_size = newMap.m_size;
			map = newMap.map;
		}

		void remove()
		{

		}

		value_type search()
		{

		}

	private:
		size_type m_size{};
		Entry* map{};
	};

}