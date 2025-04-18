#include <string>
#include <cassert>
#include <vector>
#include <queue>
#include <unordered_map>
#include <iostream>


namespace stu
{
	class bdd
	{
	public:

		struct Node
		{
			std::string formula{};
			char var{};

			Node* parent{};
			Node* low{};
			Node* high{};

			Node() = default;

			Node(const std::string& f, char v) :
				formula(f),
				var(v)
			{
			}

			bool operator ==(const Node& other)
			{
				return formula == other.formula;
			}

			bool isLeaf() const
			{
				assert((low == nullptr && high == nullptr) || (low != nullptr && high != nullptr));
				return low == nullptr || high == nullptr;
			}

			bool isTerminal() const
			{
				return formula == "0" || formula == "1";
			}

			bool isEmpty() const
			{
				return formula.empty();
			}

			bool isRoot() const
			{
				return parent == nullptr;
			}

			bool getValue() const
			{
				assert(isTerminal());
				assert(formula == "1" || formula == "0");

				return formula == "1";
			}

			// Add a part to a formula and connect with '+' sign
			void add(std::string part)
			{
				if (formula.empty())
				{
					formula = part;
					return;
				}

				formula += '+';
				formula += part;
			}

			void replaceChild(Node* old, Node* newChild)
			{
				if (old == low)
				{
					low = newChild;
				}
				else if (old == high)
				{
					high = newChild;
				}
				else
				{
					assert(false);
				}

				newChild->parent = this;
			}
		};

	private:

		using size_type = size_t;

		std::vector<std::string> split(const std::string& str, char delimeter, int reserve = 100)
		{
			std::vector<std::string> result;
			result.reserve(reserve);

			size_type lastIndex = 0;
			size_type currentIndex = 0;

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

		void removeChars(std::string& str, char ch)
		{
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

		void removeVars(std::string& part, char ch)
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

		bool hasOnly(const std::string& str, char symbol)
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

	public:

		bool isNegative(const std::string& str, char symbol)
		{
			auto pos = str.find(symbol);
			return pos != std::string::npos && pos > 0 && str[pos - 1] == '!';
		}

	private:

		Node* evaluate(const std::string& formula, char currentVar, bool value)
		{
			std::vector<std::string> parts = split(formula, '+');
			Node* result = new Node();

			// handle AND
			// here formula is the part and looks like A!BC
			if (parts.size() == 1)
			{
				std::string part = parts[0];

				bool isNeg = isNegative(part, currentVar);
				bool hasVar = part.contains(currentVar);

				if (!hasVar)
				{
					result->formula = formula;
					return result;
				}

				// try to evaluate to true
				if ((value && !isNeg) || (!value && isNeg))
				{
					removeVars(part, currentVar);

					if (part.size() == 0)
					{
						delete result;
						return &terminalTrue;
					}

					result->add(part);
				}
				else
				{
					delete result;
					return &terminalFalse;
				}

				return result;
			}

			// handle OR
			// here formula is a sum and looks like AB + !BC
			for (std::string part : parts)
			{
				bool hasVar = part.contains(currentVar);
				bool isNeg = isNegative(part, currentVar);

				if (!hasVar)
				{
					result->add(part);
				}
				else if (hasVar)
				{
					// try to evaluate the expression to true
					// evaluation to fasle will be handled later in AND
					if (hasOnly(part, currentVar) && ((value && !isNeg) || (!value && isNeg)) )
					{
						delete result;
						return &terminalTrue;
					}

					if (value != isNeg)
					{
						removeVars(part, currentVar);
						result->add(part);
					}
				}
			}

			if (!result->isEmpty())
			{
				return result;
			}

			return &terminalFalse;
		}

		void clear(Node* node)
		{
			if (!node)
			{
				return;
			}

			clear(node->low);
			clear(node->high);

			if (!node->isTerminal())
			{
				delete node;
			}
		}

	public:

		bdd(const std::string& formula, const std::string& inputOrder)
		{
			order.reserve(100);
			create(formula, inputOrder);
		}

		~bdd()
		{
			clear(root);
		}

		void traverse(Node* node, int depth = 0)
		{
			std::cout << "Node formula: " << node->formula << " depth: " << depth << "\n";
			if (node->low)
			{
				std::cout << "Node low: \n";
				traverse(node->low, depth + 1);
			}

			if (node->high)
			{
				std::cout << "Node high: \n";
				traverse(node->high, depth + 1);
			}
		}

		// recursive function to redcue the bdd
		void reduce()
		{

		}

		void create(const std::string& inputFormula, const std::string& inputOrder)
		{
			if (inputFormula.size() == 0)
			{
				return;
			}

			formula = inputFormula;
			order = inputOrder;

			// Remove spaces
			removeChars(order, ' ');
			removeChars(formula, ' ');

			// Node of the current layer
			root = new Node(formula, inputOrder[0]);
			std::queue<Node*> nodes;
			nodes.push(root);

			for (int i = 0; i < inputOrder.size(); i++)
			{
				int layerSize = nodes.size();
				char currentVar = inputOrder[i];

				for (int i = 0; i < layerSize; i++)
				{
					Node* node = nodes.front();
					node->var = currentVar;

					nodes.pop();

					Node* low = evaluate(node->formula, currentVar, false);
					Node* high = evaluate(node->formula, currentVar, true);

					// reduction Type S
					// in case that low and high are the same
					// current node appears uselless, delete it
					// and place low or high on it's place 
					//if (low == high)
					//{
					//	if (node->isRoot())
					//	{
					//		root = low;
					//	}
					//	else
					//	{	
					//		node->parent->replaceChild(node, low);
					//	}

					//	if (!low->isTerminal())
					//	{
					//		nodes.push(low);

					//		// only delete high when it's not terminal !!!
					//		// otherwise member terminal node is deleted
					//		delete high;
					//	}

					//	delete node;

					//	continue;
					//}

					if (!low->isTerminal())
					{
						nodes.push(low);
					}

					if (!high->isTerminal())
					{
						nodes.push(high);
					}

					low->parent = node;
					high->parent = node;

					node->low = low;
					node->high = high;
				}
			}
		}

		void createBestOrder()
		{

		}

		bool use(std::string input) const
		{
			if (!root)
			{
				return false;
			}

			if (input.size() != order.size())
			{
				std::cout << "\nInvalid input, expected length of: " << order.size() << ". Provided " << input.size() << "\n\n";
				return false;
			}

			// key is and input letter
			// and value is it's value 0/1
			std::unordered_map<char, int> varToValue;

			for (int i = 0; i < input.size(); i++)
			{
				char var = order[i];
				int value = input[i] - '0';

				varToValue[var] = value;
			}

			Node* current = root;

			while (!current->isTerminal())
			{
				char var = current->var;

				if (varToValue[var] == 0)
				{
					current = current->low;
				}
				else
				{
					current = current->high;
				}
			}

			return current->getValue();
		}

		Node* getRoot()
		{
			return root;
		}


	private:
		std::string formula{};
		std::string order{};

		Node* root{};

		Node terminalFalse{"0", 0};
		Node terminalTrue{"1", 0};
	};
}