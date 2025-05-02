#pragma once

#include <string>
#include <cassert>
#include <unordered_map>
#include <set>
//#include "hashtable.hpp"
//#include "list.hpp"
#include "utils.hpp"

namespace stu
{
	class bdd2
	{
	public:

		struct Node;

		struct NodeFormulaLengthCompare
		{
			// Longer formulas come first
			bool operator()(const Node* a, const Node* b) const
			{
				assert(a);
				assert(b);

				if (a->formula.size() != b->formula.size())
				{
					return a->formula.size() > b->formula.size();
				}

				// avoid duplicates becauuse comp(a,b) and comp(b,a) both false 
				return a->formula < b->formula;
			}
		};

		struct Node
		{
			std::string formula{};
			char var{};

			//list<Node*> parents{};
			std::set<Node*, NodeFormulaLengthCompare> parents{};
			Node* low{};
			Node* high{};

			Node() = default;

			Node(std::string formula) :
				formula{ std::move(formula) }
			{
			}

			Node(std::string formula, Node* parent) :
				formula{ std::move(formula) }
			{
				parents.insert(parent);
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

			bool isEvaluated() const
			{
				return low && high;
			}

			bool getValue()
			{
				return formula == "1";
			}
		};


	public:

		bdd2(std::string formula, std::string order)
		{
			create(std::move(formula), std::move(order));
		}

		~bdd2()
		{
			clear();
		}

		void clear()
		{
			for (auto& [formula, node] : m_nodes)
			{
				delete node;
			}
		}

		void create(std::string formula, std::string order)
		{
			formula = sortDnf(formula);

			trim(order);
			trim(formula);

			assert(countUniqueVariables(formula) == countUniqueVariables(order));

			m_order = std::move(order);
			m_formula = std::move(formula);

			m_root = new Node(m_formula);
			m_nodes[m_root->formula] = m_root;

			createRecursive(m_root, 0);
		}

		bool use(std::string input) const
		{
			if (!m_root)
			{
				return false;
			}

			if (input.size() != m_order.size())
			{
				return false;
			}

			size_t curVarIndex = 0;
			Node* current = m_root;

			while (!current->isTerminal())
			{
				char curVar = m_order[curVarIndex];
				bool curVarValue = input[curVarIndex] == '1' ? true : false;

				if (curVar == current->var)
				{
					if (curVarValue)
					{
						current = current->high;
					}
					else
					{
						current = current->low;
					}
				}

				curVarIndex++;
			}

			return current->getValue();
		}

		const Node* getRoot()
		{
			return m_root;
		}

	private:

	/*	void clear(Node* node)
		{
			if (!node)
			{
				return;
			}

			clear(node->low);
			clear(node->high);

			delete node;
		}*/

		void reduceTypeS(Node* parent)
		{
			Node* low = parent->low;
			Node* high = parent->high;

			if (low == high)
			{
				// Move parents from parent to temporary set
				// Not to left directly as it may be deleted further in the recursion
				std::set<Node*, NodeFormulaLengthCompare> p_parents = parent->parents;
				low->parents = std::move(parent->parents);

				// Substitute parent in other nodes (it's parents)
				for (Node* parent_p : p_parents)
				{
					if (parent_p->low == parent)
					{
						parent_p->low = low;
					}
					else
					{
						parent_p->high = low;
					}
				}

				if (parent == m_root)
				{
					m_root = low;
				}

				m_nodes.erase(parent->formula);
				parent->high = nullptr;
				parent->low = nullptr;

				delete parent;

				for (Node* parent_p : p_parents)
				{
					reduceTypeS(parent_p);
				}
			}
		}

		// creates and adds node to m_nodes if such node does not exist yet
		Node* nodeFromFormula(const std::string& formula, Node* parent)
		{
			Node* node = nullptr;

			if (m_nodes.contains(formula))
			{
				node = m_nodes[formula];
				node->parents.insert(parent);
			}
			else
			{
				node = new Node(formula, parent);
				m_nodes[formula] = node;
			}

			return node;
		}

		void createRecursive(Node* parent, int order)
		{
			if (parent->isTerminal() || parent->isEvaluated())
			{
				return;
			}

			// skip variable if it is not in the formula
			do
			{
				if (order >= m_order.size())
				{
					return;
				}

				parent->var = m_order[order];
				++order;

			} while (!parent->formula.contains(parent->var));


			std::string lowFormula = evaluate(parent->formula, parent->var, false);
			std::string highFormula = evaluate(parent->formula, parent->var, true);

			Node* low = nodeFromFormula(lowFormula, parent);
			Node* high = nodeFromFormula(highFormula, parent);

			parent->low = low;
			parent->high = high;

			reduceTypeS(parent);

			createRecursive(low, order);
			createRecursive(high, order);
		}


	private:
		Node* m_root;
		std::string m_formula;
		std::string m_order;

		std::unordered_map<std::string, Node*> m_nodes;
	};
}