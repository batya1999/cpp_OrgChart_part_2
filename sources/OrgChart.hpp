#include <string>
#include <vector>

#define DEBUG 0

const int LEVEL = 0;
const int REVERSE = 1;
const int PRE = 2;
const int MAX_INT = 2147483647;

using std::string;
using std::vector;

namespace ariel {

    struct Node {

        string value;
        Node *parent;
        bool visited;
        vector<Node *> children;

        Node(const std::string &val)
                : value(val), parent(nullptr), visited(false) {
        }

        Node(const Node *ot)
                : value(ot->value), parent(nullptr), visited(false){
            // To ensure deep copy of children vector,
            // define new node for each child, and do the same for the children.
            initNode(this, ot);
        }

        void initNode(Node *curr, const Node *ot) {
            for (Node *child: ot->children) {
                Node *c = new Node(child->value);
                c->parent = curr;
                curr->children.push_back(c);
                initNode(c, child);
            }
        }
    };

    class iterator {
    private:
        Node *ptr_curr;
        Node *root;
        int mode;

        // https://www.geeksforgeeks.org/find-deepest-node-binary-tree/
        // implemented iterations with help of link above
        /**
         * @brief Resets all of the nodes visited status.
         *
         * @param curr
         */
        void mark_all_unvisited(Node *curr);

        /**
         * @brief Returns the left-most unvisited node. [Preorder]
         *
         * @param curr
         * @return Node*
         */
        Node *find_unvisited_left_most(Node *curr);

        /**
         * @brief Returns the left-most and highest unvisited node. [Level]
         *
         * @param curr
         * @param level
         * @param minLevel
         * @param ans
         */
        void find_shallowest_left_child_rec(Node *curr, int level, int &minLevel, Node **ans);

        /**
         * @brief Returns the left-most and highest unvisited node, by calling the function above [Level]
         *
         * @return Node*
         */
        Node *find_shallowest_left_child();

        /**
         * @brief Given a current node, returns the next unvisited node by level order.
         *
         * @param curr
         * @return Node*
         */
        Node *find_next_level(Node *curr);

        /**
         * @brief Returns the left-most, deepest node. [Reverse]
         *
         * @param curr
         * @param level
         * @param maxLevel
         * @param ans
         */
        void get_deepest_left(Node *curr, int level, int &maxLevel, Node **ans);

        /**
         * @brief Returns the left-most, deepest node. [Reverse]
         *
         * @return Node*
         */
        Node *find_next_reverse();

    public:
        iterator(Node *ptr = nullptr, int m = 0)
                : ptr_curr(ptr), root(ptr), mode(m) {
            if (ptr_curr != nullptr) {
                mark_all_unvisited(ptr_curr);

                if (m != REVERSE) {
                    ptr_curr->visited = true;
                } else {
                    ptr_curr = find_next_reverse();
                    ptr_curr->visited = true;
                }
            }
        }

        std::string &operator*() {
            return ptr_curr->value;
        }

        std::string *operator->() {
            return &(ptr_curr->value);
        }

        iterator &operator++() {
            /*    A
                B   C
              D
            */
            if (ptr_curr == nullptr) {
                return *this;
            }
            if (mode == LEVEL) // A B C D
            {
                ptr_curr = find_next_level(ptr_curr);
            }
            if (mode == PRE) // A B D C
            {
                ptr_curr = find_unvisited_left_most(ptr_curr);
            }
            if (mode == REVERSE) // D B C A
            {
                ptr_curr = find_next_reverse();
            }
            return *this;
        }

        iterator operator++(int) {
            iterator temp = *this;
            operator++();
            return temp;
        }

        bool operator==(const iterator &other) const {
            return ptr_curr == other.ptr_curr;
        }

        bool operator!=(const iterator &other) const {

            return this->ptr_curr != other.ptr_curr;
        }
    };

    class OrgChart {

    protected:
        Node* first;
    public:


        OrgChart();
        OrgChart(const OrgChart &ot);
        OrgChart(OrgChart &&ot) noexcept;
        ~OrgChart();

        // Methods

        OrgChart&              operator=(const OrgChart &ot);
        OrgChart&              operator=(OrgChart &&ot) noexcept;

        void      isValid();
        void      clear(Node *curr);
        OrgChart& add_root(const std::string &r);
        void      show(Node *n, const std::string &prefix, bool isLast) const;
        OrgChart& add_sub(const std::string &dad_name, const std::string &name);
        Node*     find(const std::string &val, Node *curr);

        iterator begin();
        iterator end();
        iterator begin_level_order();
        iterator end_level_order();
        iterator begin_reverse_order();
        iterator reverse_order();
        iterator begin_preorder();
        iterator end_preorder();

        friend std::ostream&   operator<<(std::ostream &out, const OrgChart &ot);
        friend std::ostream&   operator<<(std::ostream &out, const OrgChart *ot);
    };


}
