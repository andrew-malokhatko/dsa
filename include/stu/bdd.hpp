#include <string>
#include <cassert>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <algorithm>


namespace stu
{
	class bdd
	{
	public:

		struct Node
		{
			std::string formula{};
			char var{};

			// After Reduction Type I node
			// can have multiple parents
			std::unordered_set<Node*> parents{};

			Node* low{};
			Node* high{};

			Node() = default;

			Node(const std::string& f, char v) :
				formula(f),
				var(v)
			{
			}

			//bool operator ==(const Node& other)
			//{
			//	return formula == other.formula;
			//}

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
				return parents.empty();
			}

			bool getValue() const
			{
				assert(isTerminal());
				assert(formula == "1" || formula == "0");

				return formula == "1";
			}

			bool containsVar(char var) const
			{
				return formula.contains(var);
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

			void addParent(Node* node)
			{
				parents.insert(node);
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

				if (!newChild->isTerminal())
				{
					newChild->addParent(this);
				}
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

		std::string sortDnf(const std::string& dnf)
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

					// no need to sort, assume it was sorted
					// on previous iteration
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

				result->formula = sortDnf(result->formula);
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
					if (hasOnly(part, currentVar) && ((value && !isNeg) || (!value && isNeg)))
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
				result->formula = sortDnf(result->formula);
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
				// if node has 2 or more parents, set other parents pointers
				// to nullptr to avoid double deletion
				for (Node* parent : node->parents)
				{
					if (node == parent->low)
					{
						parent->low = nullptr;
					}
					if (node == parent->high)
					{
						parent->high = nullptr;
					}
				}

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

		// queue is nodes that require further calculations
		bool reduceTypeI(Node* node)
		{
			bool reduced = false;

			// make function suitable for both low and high nodes
			if (!node->isTerminal() && allNodes.contains(node->formula))
			{
				// delete old node, and substitute it with exitsing node from allNodes
				Node* oldNode = node;
				node = allNodes[node->formula];

				// move all parents of oldNode to existing node
				for (Node* parent : oldNode->parents)
				{
					assert(oldNode == parent->low || oldNode == parent->high);

					Node** insertPos = (oldNode == parent->low) ? &parent->low : &parent->high;

					node->addParent(parent);
					*insertPos = node;

					reduced = true;
				}

				delete oldNode;
			}

			return reduced;
		}

		// in case that low and high are the same
		// current node appears uselless, delete it
		// and place low or high on it's place 
		// return value signals success of the reduction
		bool reduceTypeS(Node* parent)
		{
			if (!parent || parent->isTerminal())
			{
				return false;
			}

			Node* low = parent->low;
			Node* high = parent->high;

			if (low->formula == high->formula)
			{
				// sort the parents from longest to shortest to prevent
				// iteration on the deleted nodes
				std::vector<Node*> parents(parent->parents.begin(), parent->parents.end());
				std::sort(parents.begin(), parents.end(),
					[](const Node* a, const Node* b)
					{
						return a->formula.size() > b->formula.size();
					});

				auto it = std::find(low->parents.begin(), low->parents.end(), parent);
				if (it != low->parents.end())
				{
					low->parents.erase(it);
				}

				if (parent->isRoot())
				{
					root = low;
				}
				else
				{
					// replace parent with low in parents parent
					for (Node* pParent : parent->parents)
					{
						pParent->replaceChild(parent, low);
					}
				}

				// low may be a pointer to high
				// so if it is, it cant be deleted
				if (!high->isTerminal() && high != low)
				{
					// only delete high when it's not terminal !!!
					// otherwise member terminal node is deleted
					delete high;
				}

				if (parent->formula != low->formula)
				{
					allNodes.erase(parent->formula);
				}
				delete parent;

				for (Node* oldParent : parents)
				{
					reduceTypeS(oldParent);
				}

				return true;
			}

			return false;
		}

		// returns true, if further reduction is needed
		// also fills the array of nodes that require evaluation
		void reduce(Node* node, std::queue<Node*>& nodes)
		{
			Node* low = node->low;
			Node* high = node->high;


			// store copy of parents in case reduceTypeS
			// fires and dletes the node
			//std::vector<Node*> parents = node->parents;
			if (low && !low->isTerminal() && !reduceTypeI(low))
			{
				nodes.push(low);
				allNodes[low->formula] = low;
			}

			bool reducedS = reduceTypeS(node);

			// when reduced with S is applied, high node is deleted
			if (!reducedS)
			{
				if (high && !high->isTerminal() && !reduceTypeI(high))
				{
					nodes.push(high);
					allNodes[high->formula] = high;
				}
			}

			// try to reduce parents
			//if (reducedS)
			//{
			//	for (Node* parent : parents)
			//	{
			//		reduceTypeS(parent);
			//	}
			//}
		}


		void create(const std::string& inputFormula, const std::string& inputOrder)
		{
			if (inputFormula.size() == 0)
			{
				return;
			}

			allNodes.clear();

			formula = inputFormula;
			order = inputOrder;

			// Remove spaces
			removeChars(order, ' ');
			removeChars(formula, ' ');

			delete root;
			root = new Node(formula, 0);
			allNodes[root->formula] = root;

			std::queue<Node*> nodes;
			nodes.push(root);

			for (int i = 0; i < inputOrder.size(); i++)
			{
				char currentVar = inputOrder[i];

				for (int k = 0, layerSize = nodes.size(); k < layerSize; k++)
				{
					Node* node = nodes.front();
					node->var = currentVar;
					nodes.pop();

					// skip the node on this iterationm, if it's not influenced
					// by current variable
					if (!node->containsVar(currentVar))
					{
						nodes.push(node);
						continue;
					}

					Node* low = evaluate(node->formula, currentVar, false);
					Node* high = evaluate(node->formula, currentVar, true);

					if (!low->isTerminal())
					{
						low->addParent(node);
					}

					if (!high->isTerminal())
					{
						high->addParent(node);
					}

					node->low = low;
					node->high = high; 

					reduce(node, nodes);
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

				assert(current);
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

		// Keep track of unique nodes
		std::unordered_map<std::string, Node*> allNodes;
		Node* root{};

		Node terminalFalse{"0", 0};
		Node terminalTrue{"1", 0};
	};
}