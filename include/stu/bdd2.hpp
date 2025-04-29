#include <string>
#include <cassert>

#include "stu/utils.hpp"

namespace stu
{
	class bdd2
	{

	public:

		struct Node
		{
			std::string formula{};
			char var{};

			Node* parent;
			Node* low;
			Node* high;

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
		}

	public:

		bdd2(std::string formula, std::string order)
		{

		}
		
		void create(std::string formula, std::string order)
		{
			formula = sortDnf(formula);

			trim(order);
			trim(formula);

			assert(countUniqueVariables(formula) == countUniqueVariables(order));

			m_order = std::move(order);
			m_formula = std::move(formula);

			m_root = new Node()

			create_recursive(root, 0);
		}

	private:

		void createRecursive(Node* parent, int order)
		{
			if (order >= m_order.size() || parent->isTerminal())
			{
				return;
			}

			parent->var = m_order[order];

			std::string lowFormula = evaluate(parent->formula, parent->var, false);
			std::string highFormula = evaluate(parent->formula, parent->var, true);

			createRecursive(lowFormula, order++);
			createRecursive(highFormula, order++);
		}
	

	private:
		Node* m_root;
		std::string m_formula;
		std::string m_order;

	}
}