#include <array>
#include <iostream>
#include <algorithm>
#include <cstdlib>

constexpr uint8_t UP = 0;
constexpr uint8_t DOWN = 1;
constexpr uint8_t LEFT = 2;
constexpr uint8_t RIGHT = 3;

constexpr uint16_t N_MAX = 700;

class Node;
std::array<std::array<Node*, N_MAX>, N_MAX> nodes = {};
std::vector<Node*> leaf_nodes = {};
std::vector<Node*> leaf_nodes_new = {};
std::array<std::array<long, N_MAX>, N_MAX> values = {};
std::array<std::array<long, N_MAX>, N_MAX> min_sums = {};

class Node
{
    public:
        Node(uint16_t x, uint16_t y, uint16_t N, Node* parent) : x(x), y(y), N(N), parent(parent)
        {
        }
        ~Node()
        {
            for( auto*& p : leaf_nodes )
            {
                if( p == this )
                {
                    p = nullptr;
                    break;
                }
            }
            for( auto*& p : leaf_nodes_new )
            {
                if( p == this )
                {
                    p = nullptr;
                    break;
                }
            }
            nodes[y][x] = nullptr;
            if( parent != nullptr )
            {
                if( parent->down == this ) parent->down = nullptr;
                else if( parent->up == this ) parent->up = nullptr;
                else if( parent->left == this ) parent->left = nullptr;
                else if( parent->right == this ) parent->right = nullptr;
            }
            delete_children();
        }

        void walk()
        {
            if( x == N - 1 && y == N - 1 )
            {
                // std::cout << "Found end " << min_sums[y][x] << std::endl;
                return;
            }

            if( y < N - 1 )
            {
                new_node( x, y + 1, down);
            }
            if( y > 0 )
            {
                new_node( x, y - 1, up);
            }
            if( x > 0 )
            {
                new_node( x - 1 , y, left);
            }
            if( x < N - 1 )
            {
                new_node( x + 1 , y, right);
            }
        }

        uint16_t x;
        uint16_t y;
        uint16_t N;
        Node* parent;
    private:
        void delete_children()
        {
            if( down )
            {
                down->parent = nullptr;
                delete down;
            }
            if( up )
            {
                up->parent = nullptr;
                delete up;
            }
            if( left )
            {
                left->parent = nullptr;
                delete left;
            }
            if( right )
            {
                right->parent = nullptr;
                delete right;
            }
        }

        void new_node(uint16_t x_, uint16_t y_, Node*& node)
        {
            if( x_ == 0 && y_ == 0 ) return;

            auto& min_sumsxy = min_sums[y_][x_];
            if( min_sumsxy == 0 || ( values[y_][x_] + min_sums[y][x] < min_sumsxy )  )
            {
                if( min_sumsxy != 0 )
                {
                    auto* n = nodes[y_][x_];
                    if( n ) delete n;
                }
                min_sumsxy = values[y_][x_] + min_sums[y][x];
                node = new Node(x_, y_, N, this);
                if( std::find(leaf_nodes_new.begin(), leaf_nodes_new.end(), node) == leaf_nodes_new.end() ) leaf_nodes_new.push_back(node);
                nodes[y_][x_] = node;
            }
        }

        Node* down = nullptr;
        Node* up = nullptr;
        Node* left = nullptr;
        Node* right = nullptr;
};

class Root
{
    public:
        Root(uint16_t N)
        {
            Node* down = new Node(0, 1, N, nullptr);
            Node* right = new Node(1, 0, N, nullptr);
            leaf_nodes_new.push_back(down);
            leaf_nodes_new.push_back(right);
            min_sums[1][0] = values[0][0] + values[1][0];
            nodes[1][0] = down;
            min_sums[0][1] = values[0][0] + values[0][1];
            nodes[0][1] = right;

            while( leaf_nodes_new.size() )
            {
                leaf_nodes = leaf_nodes_new;
                // std::cerr << leaf_nodes.size() << std::endl;
                leaf_nodes_new.clear();
                for( auto* c : leaf_nodes )
                {
                    if( c )
                    {
                        c->walk();
                    }
                }
            }
        }
};

void create_random_field( int N )
{
	// Providing a seed value
	srand((unsigned) time(NULL));

    for( int i = 0; i < N; i++ )
    {
        auto& valsi = values[i];
        for(int j = 0; j < N; j++ )
        {
            valsi[j] = rand() % 10;
            // std::cout << valsi[j] << " ";
        }
        // std::cout << std::endl;
    }
}

int main()
{
    const int N = 5;
    /*std::vector<std::vector<int>> vals = {
        {131, 673, 234, 103, 18},
        {201, 96, 342, 965, 150},
        {630, 803, 746, 422, 111},
        {537, 699, 497, 121, 956},
        {805, 732, 524, 37, 331},
    };
    for( int i = 0; i < N; i++ )
    {
        auto& valsi = values[i];
        for(int j = 0; j < N; j++ )
        {
            valsi[j] = vals[i][j];
            std::cout << valsi[j] << " ";
        }
        std::cout << std::endl;
    }*/
    //create_random_field(N);
    Root r(N);
    std::cout << min_sums[N-1][N-1]<< std::endl;
    return 0;
}