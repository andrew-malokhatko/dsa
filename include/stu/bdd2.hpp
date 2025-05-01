#pragma once

#include <string>
#include <cassert>
#include <unordered_map>
#include "hashtable.hpp"
#include "list.hpp"
#include "utils.hpp"

namespace stu
{
	class bdd2
	{
	public:

		struct Node
		{
			std::string formula{};
			char var{};

			list<Node*> parents{};
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

		void create(std::string formula, std::string order)
		{
			formula = sortDnf(formula);

			trim(order);
			trim(formula);

			assert(countUniqueVariables(formula) == countUniqueVariables(order));

			m_order = std::move(order);
			m_formula = std::move(formula);

			m_root = new Node(m_formula);

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

		void reduceTypeS(Node* parent)
		{
			Node* low = parent->low;
			Node* high = parent->high;

			if (low == high)
			{
				// Move parents from parent to left
				low->parents = std::move(parent->parents);

				// Substitute parent in other nodes (it's parents)
				Node* current = low->parents.first();

				while (current)
				{
					if (current->low == parent)
					{
						current->low = parent;
					}
					else
					{
						current->high = parent;
					}
				}
				
				// delete parent
				parent->high = nullptr;
				parent->low = nullptr;

				m_nodes.erase(parent->formula);

				delete parent;
			}

			// recursion

		}

		// creates and adds node to m_nodes if such node does not exist yet
		Node* nodeFromFormula(const std::string& formula, Node* parent)
		{
			Node* node = nullptr;

			if (m_nodes.contains(formula))
			{
				node = m_nodes[formula];
				node->parents.insert(parent);
				//node = m_nodes.search(formula);
				//node.addParent(parent);
			}
			else
			{
				node = new Node(formula, parent);
				m_nodes[formula] = node;
				//m_nodes.insert(formula, node);
			}

			return node;
		}

		void createRecursive(Node* parent, int order)
		{
			if (order >= m_order.size() || parent->isTerminal())
			{
				return;
			}

			parent->var = m_order[order];

			std::string lowFormula = evaluate(parent->formula, parent->var, false);
			std::string highFormula = evaluate(parent->formula, parent->var, true);

			Node* low = nodeFromFormula(lowFormula, parent);
			Node* high = nodeFromFormula(highFormula, parent);

			parent->low = low;
			parent->high = high;

			reduceTypeS(parent);

			++order;
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