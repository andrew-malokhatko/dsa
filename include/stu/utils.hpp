#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include <algorithm>


namespace stu
{
	inline std::vector<std::string> split(const std::string& str, char delimeter, int reserve = 100)
	{
		std::vector<std::string> result;
		result.reserve(reserve);

		size_t lastIndex = 0;
		size_t currentIndex = 0;

		for (char ch : str)
		{
			if (ch == delimeter)
			{
				std::string variables = str.substr(lastIndex, currentIndex - lastIndex);
				result.emplace_back(variables);

				// Skip the delimeter
				lastIndex = currentIndex + 1;
			}

			currentIndex++;
		}

		// Maybe review !!!
		result.emplace_back(str.substr(lastIndex, currentIndex));

		return result;
	}

	// Modifies the string
	inline void trim(std::string& str)
	{
		static constexpr char ch = ' ';

		size_t write = 0;
		for (size_t read = 0; read < str.length(); ++read)
		{
			if (str[read] != ch)
			{
				str[write++] = str[read];
			}
		}

		str.resize(write);
	}

	inline void removeVars(std::string& part, char ch)
	{
		size_t write = 0;
		for (size_t read = 0; read < part.length(); ++read)
		{
			if (part[read] != ch)
			{
				part[write++] = part[read];
			}
			// remove negation with the removed variable
			else if (read != 0 && part[read - 1] == '!')
			{
				write--;
			}
		}

		part.resize(write);
	}


	inline std::string sortDnf(const std::string& dnf)
	{
		std::vector<std::string> terms = split(dnf, '+');

		for (std::string& term : terms)
		{
			std::vector<char> literals;
			std::vector<bool> negations;

			// Extract literals and their negation status
			for (size_t i = 0; i < term.length(); ++i)
			{
				if (term[i] == '!')
				{
					++i;
					if (i < term.length())
					{
						literals.push_back(term[i]);
						negations.push_back(true);
					}
				}
				else
				{
					literals.push_back(term[i]);
					negations.push_back(false);
				}
			}

			// Sort literals while maintaining negation status
			std::vector<std::pair<char, bool>> literal_pairs;
			for (size_t i = 0; i < literals.size(); ++i)
			{
				literal_pairs.emplace_back(literals[i], negations[i]);
			}

			std::sort(literal_pairs.begin(), literal_pairs.end(),
				[](const auto& a, const auto& b) {
					return a.first < b.first;
				});

			// Reconstruct sorted term
			std::string sorted_term;
			for (const auto& pair : literal_pairs)
			{
				if (pair.second)
				{
					sorted_term += '!';
				}
				sorted_term += pair.first;
			}
			term = sorted_term;
		}

		// Sort terms lexicographically
		std::sort(terms.begin(), terms.end());

		// Join terms back together
		std::string result;
		for (size_t i = 0; i < terms.size(); ++i)
		{
			result += terms[i];
			if (i < terms.size() - 1)
			{
				result += '+';
			}
		}

		return result;
	}

	inline bool isalpha(char ch)
	{
		return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
	}

	inline bool countUniqueVariables(std::string formula)
	{
		std::unordered_set<char> uniqueChars;

		for (char ch : formula)
		{
			if (isalpha(ch))
			{
				uniqueChars.insert(ch);
			}
		}

		return uniqueChars.size();
	}

	inline bool hasOnly(const std::string& str, char symbol)
	{
		for (int i = 0; i < str.size(); i++)
		{
			if (str[i] != symbol && str[i] != '!')
			{
				return false;
			}
		}
		return true;
	}

	inline void addAsDisjunction(std::string& formula, std::string& right)
	{
		if (formula.empty())
		{
			formula = right;
			return;
		}

		formula += '+';
		formula += right;
	}

	inline bool isNegative(const std::string& str, char symbol)
	{
		auto pos = str.find(symbol);
		return pos != std::string::npos && pos > 0 && str[pos - 1] == '!';
	}

	inline std::string evaluate(const std::string& formula, char currentVar, bool value)
	{
		std::vector<std::string> parts = split(formula, '+');

		// handle empty string
		if (parts.empty())
		{
			return "0";
		}

		// handle AND operation
		if (parts.size() == 1)
		{
			std::string& part = parts[0];

			bool isNeg = isNegative(part, currentVar);
			bool hasVar = part.contains(currentVar);

			if (!hasVar)
			{
				return part;
			}

			// if variable is positive
			if (value != isNeg)
			{
				removeVars(part, currentVar);

				return part.size() == 0 ? "1" : part;
			}
			else
			{
				return "0";
			}
		}


		std::string result{};

		for (std::string& part : parts)
		{
			bool hasVar = part.contains(currentVar);
			bool isNeg = isNegative(part, currentVar);

			if (!hasVar)
			{
				addAsDisjunction(result, part);
			}
			else
			{
				// if variable is positive
				if (value != isNeg)
				{
					// if is is the only variable in disjunction and it is positive
					if (hasOnly(part, currentVar))
					{
						return "1";
					}

					removeVars(part, currentVar);
					addAsDisjunction(result, part);
				}
			}
		}

		if (result.empty())
		{
			return "0";
		}

		return result;
	}

	inline bool isPrime(int n)
	{
		if (n <= 1) return false;
		for (int i = 2; i * i <= n; ++i)
			if (n % i == 0)
				return false;
		return true;
	}

	// Returns next prime after n or n if it is already prime
	inline int nextPrime(int n)
	{
		while (!isPrime(n)) ++n;
		return n;
	}

	inline size_t pow(int n, int pow)
	{
		int res = 1;
		for (int i = 0; i < pow; i++)
		{
			res *= n;
		}
		return res;
	}
}