//
//namespace stu
//{
//	template<typename value_type>
//	class set
//	{
//		struct Node
//		{
//			value_type value;
//
//			Node* left{};
//			Node* right{};
//
//			Node(value_type value) :
//				value{ value }
//			{
//			}
//
//			~Node()
//			{
//				delete left;
//				delete right;
//			}
//		};
//
//	public:
//		set()
//		{
//
//		}
//
//		~set()
//		{
//
//		}
//
//		void insert(const value_type& value)
//		{
//			if (!m_root)
//			{
//				m_root = new Node(value);
//				return;
//			}
//
//			Node* current = m_root;
//
//			while (current)
//			{
//				if (value < current->value)
//				{
//					if (!current->left)
//					{
//						current->left = new Node(value);
//						return;
//					}
//
//					current = current->left;
//					continue;
//				}
//
//				if (current->value < value)
//				{
//					if (!current->right)
//					{
//						current->right = new Node(value);
//						return;
//					}
//
//					current = current->right;
//					continue;
//				}
//
//				return;
//			}
//		}
//
//		Node* getPredecessor(Node* node)
//		{
//			Node* current = node->left;
//			while (current)
//			{
//				current = current->right;
//			}
//		}
//
//		bool remove(const value_type& value)
//		{
//			Node* current = m_root;
//			Node* parent = nullptr;
//
//			while (current)
//			{
//				if (value < current->value)
//				{
//					current = current->left;
//					continue;
//				}
//
//				if (current->value < value)
//				{
//					parent = current;
//					current = current->right;
//					continue;
//				}
//
//				if ()
//			}
//		}
//
//		void contains(const value_type& value) const
//		{
//
//		}
//
//	private:
//		size_t size{};
//		Node* m_root{};
//	};
//}