#include <string>
#include <cassert>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <iostream>
#include <algorithm>
#include <memory>


namespace stu
{
	class bdd
	{
	public:
		class Node;
		using NodePtr = std::shared_ptr<Node>;
		using NodeWeakPtr = std::weak_ptr<Node>;

		struct Node : std::enable_shared_from_this<Node>
		{
			std::string formula{};
			char var{};

			struct WeakPtrLess {
				bool operator()(const NodeWeakPtr& lhs, const NodeWeakPtr& rhs) const {
					return lhs.lock().get() < rhs.lock().get();
				}
			};

			// After Reduction Type I node
			// can have multiple parents
			std::set<NodeWeakPtr, WeakPtrLess> parents{};

			NodePtr low{};
			NodePtr high{};

			Node() = default;

			Node(const std::string& f, char v) :
				formula(f),
				var(v)
			{
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

			void addParent(NodePtr node)
			{
				parents.insert(node);
			}

			void replaceChild(NodePtr old, NodePtr newChild)
			{
				assert(old == low || old == high);

				if (old == low)
				{
					low = newChild;
				}
				else
				{
					high = newChild;
				}

				if (newChild->isTerminal() == false)
				{
					newChild->addParent(this->shared_from_this());
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
		NodePtr evaluate(const std::string& formula, char currentVar, bool value)
		{
			std::vector<std::string> parts = split(formula, '+');
			auto result = std::make_shared<Node>();

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
						return m_terminalTrue;
					}

					result->add(part);
				}
				else
				{
					return m_terminalFalse;
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
					// evaluation to false will be handled later in AND
					if (hasOnly(part, currentVar) && ((value && !isNeg) || (!value && isNeg)))
					{
						return m_terminalTrue;
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

			return m_terminalFalse;
		}

	public:

		bdd(const std::string& formula, const std::string& inputOrder)
		{
			m_order.reserve(100);
			create(formula, inputOrder);
		}

		static void traverse(const NodePtr& node, int depth = 0)
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
		bool reduceTypeI(NodePtr node)
		{
			bool reduced = false;

			// make function suitable for both low and high nodes
			if (!node->isTerminal() && m_allNodes.contains(node->formula))
			{
				// delete old node, and substitute it with existing node from allNodes
				auto oldNode = node;
				node = m_allNodes[node->formula];

				// move all parents of oldNode to existing node
				for (auto parent : oldNode->parents)
				{
					NodePtr parentPtr = parent.lock();
					assert(parentPtr);

					assert(oldNode == parentPtr->low || oldNode == parentPtr->high);

					if (oldNode == parentPtr->low)
					{
						parentPtr->low = node;
					}
					else
					{
						parentPtr->high = node;
					}

					node->addParent(parentPtr);

					reduced = true;
				}
			}

			return reduced;
		}

		// in case that low and high are the same
		// current node appears useless, delete it
		// and place low or high on it's place 
		// return value signals success of the reduction
		bool reduceTypeS(NodePtr parent)
		{
			if (!parent || parent->isTerminal())
			{
				return false;
			}

			NodePtr low = parent->low;
			NodePtr high = parent->high;

			if (low->formula == high->formula)
			{
				// sort the parents from longest to shortest to prevent
				// iteration on the deleted nodes
				std::vector<NodeWeakPtr> parents(parent->parents.begin(), parent->parents.end());
				std::sort(parents.begin(), parents.end(),
					[](const NodeWeakPtr& a, const NodeWeakPtr& b)
					{
						return a.lock()->formula.size() > b.lock()->formula.size();
					});

				NodeWeakPtr weakParent{ parent };
				low->parents.erase(weakParent);

				if (parent->isRoot())
				{
					m_root = low;
				}
				else
				{
					// replace parent with low in parents parent
					for (NodeWeakPtr pParent : parent->parents)
					{
						pParent.lock()->replaceChild(parent, low);
					}
				}

				if (parent->formula != low->formula)
				{
					m_allNodes.erase(parent->formula);
				}

				for (auto oldParent : parents)
				{
					reduceTypeS(oldParent.lock());
				}

				return true;
			}

			return false;
		}

		// returns true, if further reduction is needed
		// also fills the array of nodes that require evaluation
		void reduce(NodePtr node, std::queue<NodePtr>& nodes)
		{
			auto low = node->low;
			auto high = node->high;


			// store copy of parents in case reduceTypeS
			// fires and deletes the node
			//std::vector<Node*> parents = node->parents;
			if (low && !low->isTerminal() && !reduceTypeI(low))
			{
				nodes.push(low);
				m_allNodes[low->formula] = low;
			}

			bool reducedS = reduceTypeS(node);

			// when reduced with S is applied, high node is deleted
			if (!reducedS)
			{
				if (high && !high->isTerminal() && !reduceTypeI(high))
				{
					nodes.push(high);
					m_allNodes[high->formula] = high;
				}
			}
		}


		void create(const std::string& inputFormula, const std::string& inputOrder)
		{
			if (inputFormula.size() == 0)
			{
				return;
			}

			m_allNodes.clear();

			m_formula = inputFormula;
			m_order = inputOrder;

			// Remove spaces
			removeChars(m_order, ' ');
			removeChars(m_formula, ' ');

			m_root = std::make_shared<Node>(m_formula, 0);
			m_allNodes[m_root->formula] = m_root;

			std::queue<NodePtr> nodes;
			nodes.push(m_root);

			for (int i = 0; i < inputOrder.size(); i++)
			{
				char currentVar = inputOrder[i];

				for (int k = 0, layerSize = nodes.size(); k < layerSize; k++)
				{
					auto node = nodes.front();
					node->var = currentVar;
					nodes.pop();

					// skip the node on this iteration, if it's not influenced
					// by current variable
					if (!node->containsVar(currentVar))
					{
						nodes.push(node);
						continue;
					}

					auto low = evaluate(node->formula, currentVar, false);
					auto high = evaluate(node->formula, currentVar, true);

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
			if (!m_root)
			{
				return false;
			}

			if (input.size() != m_order.size())
			{
				std::cout << "\nInvalid input, expected length of: " << m_order.size() << ". Provided " << input.size() << "\n\n";
				return false;
			}

			// key is and input letter
			// and value is it's value 0/1
			std::unordered_map<char, int> varToValue;

			for (int i = 0; i < input.size(); i++)
			{
				char var = m_order[i];
				int value = input[i] - '0';

				varToValue[var] = value;
			}

			auto current = m_root;

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

		NodePtr& root()
		{
			return m_root;
		}

		const NodePtr& root() const
		{
			return m_root;
		}

	private:
		std::string m_formula{};
		std::string m_order{};

		// Keep track of unique nodes
		std::unordered_map<std::string, NodePtr> m_allNodes;
		NodePtr m_root{};

		NodePtr m_terminalFalse = std::make_shared<Node>("0", 0);
		NodePtr m_terminalTrue = std::make_shared<Node>("1", 0);
	};
}