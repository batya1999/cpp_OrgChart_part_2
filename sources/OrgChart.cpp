#include <iostream>
#include "OrgChart.hpp"
#include <iostream>

using namespace ariel;


OrgChart::OrgChart() : first(nullptr) {

}

OrgChart::OrgChart(const OrgChart &ot) {
    first = new Node(ot.first);
}

OrgChart::OrgChart(OrgChart &&ot) noexcept{
first = ot.first;
ot.first = nullptr;
}

OrgChart::~OrgChart() {
    delete first;
}



OrgChart& OrgChart::operator=(const OrgChart &ot) {

    if (this == &ot) {
        return *this;
    }

    // Delete current, deep copy from other to self, return self.

    clear(first);
    if (ot.first != nullptr) {
        first = new Node(ot.first);
    }
    return *this;
}

OrgChart& OrgChart::operator=(OrgChart &&ot) noexcept // delcaration that the function will not throw an exception
{
    if (this == &ot) {
        return *this;
    }
    // Move resources without creating additional copies

    first = ot.first;
    ot.first = nullptr;
    return *this;
}

std::ostream& ariel::operator<<(std::ostream &out, const OrgChart &ot) {
    ot.show(ot.first, "", false);
    return out;
}

std::ostream& ariel::operator<<(std::ostream &out, const OrgChart *ot) {
    ot->show(ot->first, "", false);
    return out;
}

OrgChart &OrgChart::add_sub(const std::string &dad_name, const std::string &name) {
    /*Check dad is in chart*/
    Node *sub = find(dad_name, first);
    if (sub == nullptr) {
        if (DEBUG) {
            std::cout << "Did not find node:" << dad_name << std::endl;
        }
        throw std::invalid_argument("add_sub(): left parameter node not in chart.");
    }

    Node *newborn = new Node(name);
    newborn->parent = sub;
    sub->children.push_back(newborn);
    return *this;
}

void OrgChart::clear(Node *curr) {
    if (curr != nullptr) {
        for (Node *child: curr->children) {
            clear(child);
        }
        delete curr;
    }
}

Node *OrgChart::find(const std::string &val, Node *curr) {
    if (curr != nullptr) {
        if (curr->value == val) {
            return curr;
        }

        if (curr->children.empty()) {
            return nullptr;
        }

        for (Node *n: curr->children) {
            Node *a = find(val, n);
            if (a != nullptr) {
                return a;
            }
        }
    }
    return nullptr;
}

void OrgChart::show(Node *n, const std::string &prefix, bool isLast) const {
    if (n != nullptr) {
        std::cout << prefix;
        if (isLast) {
            std::cout << "+-- ";
        } else {
            std::cout << "+-- ";
        }
        std::cout << n->value << std::endl;

        for (size_t i = 0; i < n->children.size(); i++) {
            if (i + 1 == n->children.size()) // LAST
            {
                show(n->children.at(i), prefix + ("|   "), true);
            } else {
                show(n->children.at(i), prefix + ("|   "), false);

            }
        }
    }
}

void OrgChart::isValid() {
    if (first == nullptr) {
        throw std::invalid_argument("Can't iterate empty org.");
    }
}

OrgChart& OrgChart::add_root(const std::string &r) {
    this->first = new Node(r);
    return *this;
}




// Default iterator, returns level
iterator OrgChart::begin() {
    isValid();
    return iterator{first, LEVEL};
}

iterator OrgChart::end() {
    isValid();
    return iterator{nullptr, LEVEL};
}

iterator OrgChart::begin_level_order() {
    isValid();
    return iterator{first, LEVEL};
}

iterator OrgChart::end_level_order() {
    isValid();
    return iterator{nullptr, LEVEL};
}

iterator OrgChart::begin_reverse_order() {
    isValid();
    return iterator{first, REVERSE};
}

iterator OrgChart::reverse_order() {
    isValid();
    return iterator{nullptr, REVERSE};
}

iterator OrgChart::begin_preorder() {
    isValid();
    return iterator{first, PRE};
}

iterator OrgChart::end_preorder() {
    isValid();
    return iterator{nullptr, PRE};
}

/* ******************** Iterator functions ******************** */


/**
 * @brief Returns the left-most and highest unvisited node.
 *
 * @param curr
 * @param level
 * @param minLevel
 * @param ans
 */
void iterator::find_shallowest_left_child_rec(Node *curr, int level, int &minLevel, Node **ans) {
    if (curr != nullptr) {
        for (Node *child: curr->children) {
            iterator::find_shallowest_left_child_rec(child, level + 1, minLevel, ans);
        }

        if (level < minLevel) {
            if (!curr->visited) {
                *ans = curr;
                minLevel = level;
            }
        }
    }
}


/**
 * @brief Resets all of the nodes visited status.
 * 
 * @param curr 
 */
void iterator::mark_all_unvisited(Node *curr) {
    if (curr != nullptr) {
        curr->visited = false;
        for (Node *child: curr->children) {
            iterator::mark_all_unvisited(child);
        }
    }
}

/**
 * @brief Returns the left-most unvisited node. [Preorder]
 *
 * @param curr
 * @return Node*
 */
Node *iterator::find_unvisited_left_most(Node *curr) {
    if (curr == nullptr) {
        return nullptr;
    }
    for (Node *child: curr->children) {
        if (!child->visited) {
            child->visited = true;
            return child;
        }
    }
    return iterator::find_unvisited_left_most(curr->parent);
}

/**
 * @brief Returns the left-most and highest unvisited node.
 * 
 * @return Node* 
 */
Node *iterator::find_shallowest_left_child() {
    Node *ans = nullptr;
    int minLevel = MAX_INT;
    iterator::find_shallowest_left_child_rec(this->root, 0, minLevel, &ans);
    if (ans == nullptr || ans->visited) {
        return nullptr;
    }
    ans->visited = true;
    return ans;
}

/**
 * @brief Given a current node, returns the next unvisited node by level order.
 * 
 * @param curr 
 * @return Node* 
 */
Node *iterator::find_next_level(Node *curr) {
    /* basic idea: 
    • return any untraveled sibling you have
    • else go up the tree and find untraveled children of a node
    • else return nullptr
    */
    if (curr == nullptr) {
        return nullptr;
    }

    if (curr->parent == nullptr) {
        for (Node *child: curr->children) {
            if (!child->visited) {
                child->visited = true;
                return child;
            }
        }
    } else {
        for (Node *sibling: curr->parent->children) {
            if (!sibling->visited) {
                sibling->visited = true;
                return sibling;
            }
        }
    }

    return iterator::find_shallowest_left_child();
}

/**
 * @brief Returns the left-most, deepest node.
 * 
 * @param curr 
 * @param level 
 * @param maxLevel 
 * @param ans 
 */
void iterator::get_deepest_left(Node *curr, int level, int &maxLevel, Node **ans) {
    if (curr != nullptr) {
        for (Node *child: curr->children) {
            iterator::get_deepest_left(child, level + 1, maxLevel, ans);
        }
        if (level > maxLevel && !curr->visited) {
            *ans = curr;
            maxLevel = level;
        }
    }
}

/**
 * @brief Returns the left-most, deepest node.
 * 
 * @return Node* 
 */
Node *iterator::find_next_reverse() {
    /* basic idea: 
    • get the deepest root in the tree, if there are several, get the one to the left
    • mark it as visited and return it
    */
    Node *ans = nullptr;
    int maxLevel = -1;
    iterator::get_deepest_left(this->root, 0, maxLevel, &ans);

    if (ans == nullptr || ans->visited) {
        return nullptr;
    }
    ans->visited = true;
    return ans;
}


