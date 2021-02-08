#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <map>
#include <algorithm>

char unifiedChar(char ch)
{
	if (ch == 'o' || ch == 'O' || ch == '0') return 'O';
	else if (ch == 'x' || ch == 'X') return 'X';
	else return '_';
}
char getSide()
{
	char side = ' ';
	while (side != 'X' && side != 'O')
	{
		std::cout << "Please enter your side (X/0): ";
		std::cin >> side;
		side = unifiedChar(side);
	}
	return side;
}
void printNode(std::string str)
{
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (i % 3 == 0)
		{
			std::cout << "\n";
		}
		std::cout << str[i];
	}
	std::cout << std::endl;
}
std::string readNode() {
	std::string line;
	std::string result = "";
	for (int i = 0; i < 3;)
	{
		getline(std::cin, line);
		if (line.size() != 3) continue;
		++i;
		for (auto ch : line) result += unifiedChar(ch);
	}
	return result;
}

enum class NodeType
{
	Winnings,
	Losing,
	Draw,
	Invalid
};

struct Node
{
	explicit Node(std::string descriptor) : m_descriptor(descriptor) {}
	NodeType m_type = NodeType::Invalid;
	std::string m_descriptor;
	std::vector<Node*> m_children;
};

class GameTree
{
public:
	GameTree() { buildTree(m_rootDescriptor); }
	~GameTree() 
	{
		for (auto node : m_nodes)
		{
			delete node.second;
			node.second = nullptr;
		}
	}

	Node* get(std::string descriptor) { return m_nodes[descriptor]; }
	const std::string m_rootDescriptor = "_________";

private:
	bool equal(const char a, const char b, const char c, const char d)
	{
		return a == b && a == c && a == d;
	}

	bool hasThreeInRow(const std::string& descriptor, const char sign)
	{
		return equal(descriptor[0], descriptor[1], descriptor[2], sign)
			|| equal(descriptor[3], descriptor[4], descriptor[5], sign)
			|| equal(descriptor[6], descriptor[7], descriptor[8], sign)

			|| equal(descriptor[0], descriptor[3], descriptor[6], sign)
			|| equal(descriptor[1], descriptor[4], descriptor[7], sign)
			|| equal(descriptor[2], descriptor[5], descriptor[8], sign)

			|| equal(descriptor[0], descriptor[4], descriptor[8], sign)
			|| equal(descriptor[2], descriptor[4], descriptor[6], sign);
	}

	Node* buildTree(const std::string& descriptor, const bool cross = true)
	{
		if (m_nodes.find(descriptor) != m_nodes.end()) return m_nodes[descriptor];
		Node* node = new Node(descriptor);
		m_nodes[descriptor] = node;

		if (hasThreeInRow(descriptor, 'X'))
		{
			node->m_type = NodeType::Winnings;
			return node;
		}
		else if (hasThreeInRow(descriptor, 'O'))
		{
			node->m_type = NodeType::Losing;
			return node;
		}

		bool hasWinnings = false;
		bool hasLosing = false;
		bool hasDraw = false;
		for (rsize_t i = 0; i < 9; ++i)
		{
			if (descriptor[i] == '_')
			{
				std::string childDescriptor = descriptor;
				childDescriptor[i] = cross ? 'X' : 'O';
				Node* child = buildTree(childDescriptor, !cross);
				node->m_children.push_back(child);
				hasWinnings |= child->m_type == NodeType::Winnings;
				hasLosing |= child->m_type == NodeType::Losing;
				hasDraw |= child->m_type == NodeType::Draw;
			}
		}

		if (node->m_children.empty())
		{
			node->m_type = NodeType::Draw;
			return node;
		}

		std::sort(node->m_children.begin(), node->m_children.end(), [](const Node* a, const Node* b)
		{
			return a->m_type < b->m_type;
		});

		if ((!hasLosing && !hasDraw) || (cross && hasWinnings)) node->m_type = NodeType::Winnings;
		else if (!hasLosing || (cross && hasDraw)) node->m_type = NodeType::Draw;
		else node->m_type = NodeType::Losing;

		return node;
	}

	std::map<std::string, Node*> m_nodes;
} g_ticTacToeGameTree;

class Player
{
public:
	virtual ~Player() {}
	void setSide(char side) { m_side = side; }
	char side() { return m_side; }
	virtual Node* makeStep(Node* node) = 0;
private:
	char m_side;
};

class User : Player
{
public:
	Node* makeStep(Node* node) final override
	{
		while (true)
		{
			std::cout << "current state:";
			printNode(node->m_descriptor);
			std::cout << "please enter next state: \n";
			std::string descriptor = readNode();
			for (Node* child : node->m_children)
			{
				if (child->m_descriptor == descriptor) return child;
			}
			std::cout << "state wrong, ";
		}
		return nullptr;
	}
};

class Computer : Player
{
public:
	Node* makeStep(Node* node) final override
	{
		return side() == 'X' ? node->m_children[0] : node->m_children[node->m_children.size() - 1];
	}
};

class TicTacToe
{
public:
	TicTacToe(Player* first, Player* second)
	{
		m_players[0] = first;
		m_players[1] = second;

		first->setSide('X');
		second->setSide('O');
	}

	int run()
	{
		while (!m_state->m_children.empty())
		{
			m_state = m_players[++m_currentPlaye %= 2]->makeStep(m_state);
		}

		std::cout << "Game over: ";
		if (m_state->m_type == NodeType::Draw) std::cout << "Draw\n";
		if (m_state->m_type == NodeType::Winnings) std::cout << "X win\n";
		if (m_state->m_type == NodeType::Losing) std::cout << "0 win\n";
		return 0;
	}
private:
	std::array<Player*, 2> m_players;
	size_t m_currentPlaye = 1;
	Node* m_state = g_ticTacToeGameTree.get(g_ticTacToeGameTree.m_rootDescriptor);
};

int main()
{
	std::cout << "Hello Tic tac toe!!!\n";

	char side = getSide();

	User user;
	Computer computer;

	Player* first = side == 'X' ? (Player*)&user : (Player*)&computer;
	Player* second = side == 'O' ? (Player*)&user : (Player*)&computer;

	TicTacToe game(first, second);
	return game.run();
}
