#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>


class TSuffixNode {
public:
    std::map<char, TSuffixNode*> to;
    std::string::iterator begin, end;
    TSuffixNode* suffixlink;
    TSuffixNode(std::string::iterator begin, std::string::iterator end);
    ~TSuffixNode() {};
};

class TSuffixTree {
public:
    TSuffixTree(std::string);
    void print();
    ~TSuffixTree() {};
    friend class TSuffixArray;
private:
    std::string text;
    int remainder;
    TSuffixNode *root;
    TSuffixNode *NeedSufLink, *activeNode;
    int activeLen;
    std::string::iterator activeEdge;

    //void NodePrint(TSuffixNode *node, int depth);
    void AddSuffLink(TSuffixNode *node);
    void TreeDestroy(TSuffixNode *node);
    bool WalkDown(std::string::iterator current_position, TSuffixNode* node);
    int EdgeLen(TSuffixNode *node, std::string::iterator position);
    void TreeExtend(std::string::iterator add);
    void DFS(TSuffixNode *node, std::vector<int> &result, int depth);
};


class TSuffixArray {
public:
    TSuffixArray(TSuffixTree);
    ~TSuffixArray() {};
    std::vector<int> Find(std::string pattern);
private:
std::string text;
std::vector<int> array;
};

TSuffixTree::TSuffixTree(std::string str): text(str), remainder(0), root(new TSuffixNode(text.end(), text.end())) {
    activeEdge = text.begin();
    activeNode = NeedSufLink = root->suffixlink = root;
    activeLen = 0;

    for(std::string::iterator it = text.begin(); it != text.end(); ++it) {
        TreeExtend(it);
    }
}

TSuffixNode::TSuffixNode(std::string::iterator begin, std::string::iterator end): begin(begin), end(end), suffixlink(0) {}

void TSuffixTree::TreeDestroy(TSuffixNode *node) {
    for(std::map<char, TSuffixNode*>::iterator it = node->to.begin(); it != node->to.end(); ++it) {
        TreeDestroy(it->second);
    }
    delete node;
}

void TSuffixTree::TreeExtend(std::string::iterator add) {
    NeedSufLink = root;
    remainder++;
while (remainder != 0) {
    if (!activeLen) {
        activeEdge = add;
    }
    std::map<char, TSuffixNode*>::iterator it = activeNode->to.find(*activeEdge);
    TSuffixNode *next;
    if (it == activeNode->to.end()) {
        next = NULL;
    } else {
        next = it->second;
    }
    if(!next) {
        TSuffixNode *leaf = new TSuffixNode(add ,text.end());
        activeNode->to[*activeEdge] = leaf;
        AddSuffLink(activeNode);
    } else {
        if (WalkDown(add, next)) { ;
            continue;
        }
        if (*(next->begin + activeLen) == *add) {
            ++activeLen;
            AddSuffLink(activeNode);
            break;
        }
        TSuffixNode *split = new TSuffixNode(next->begin, next->begin + activeLen);
        TSuffixNode *leaf = new TSuffixNode(add, text.end());
        activeNode->to[*activeEdge] = split;
        split->to[*add] = leaf;
        next->begin += activeLen;
        split->to[*next->begin] = next;
        AddSuffLink(split);
    }
    --remainder;
    if (activeNode == root && activeLen != 0) {
        --activeLen;
        activeEdge = add - remainder + 1;
    } else {
        if (activeNode->suffixlink != 0) {
            activeNode = activeNode->suffixlink;
        } else {
            activeNode = root;
            }
        }
    }

}

int TSuffixTree::EdgeLen(TSuffixNode *node, std::string::iterator position) {
    return min(node->end, position + 1) - node->begin;
}

bool TSuffixTree::WalkDown(std::string::iterator current_position, TSuffixNode *node) {
    if(activeLen >= EdgeLen(node, current_position)) {
        activeEdge += EdgeLen(node, current_position);
        activeLen -= EdgeLen(node, current_position);
        activeNode = node;
        return true;
    } else {
        return false;
    }
}

void TSuffixTree::AddSuffLink(TSuffixNode *node) {
    if(NeedSufLink != root) {
        NeedSufLink->suffixlink = node;
    }
    NeedSufLink = node ;
}

void TSuffixTree::DFS(TSuffixNode *node, std::vector<int> &result, int depth) {
    if(node->to.empty()) {
        result.push_back(text.size() - depth);
        return;
    }

    for(std::map<char, TSuffixNode*>::iterator it = node->to.begin(); it != node->to.end(); ++it) {
        int temp = depth;
        temp += it->second->end - it->second->begin;
        DFS(it->second, result, temp);
    }
}

TSuffixArray::TSuffixArray(TSuffixTree tree): text(tree.text), array() {
    tree.DFS(tree.root, array, 0);
    tree.TreeDestroy(tree.root);
}

std::vector<int> TSuffixArray::Find(std::string pattern) {
std::pair<std::vector<int>::iterator, std::vector<int>::iterator> range(array.begin(), array.end());
for(int i = 0; i < pattern.size() && range.first != range.second; ++i) {
    range = equal_range(range.first, range.second, std::numeric_limits<int>::max(), [this, &pattern, &i](int idx1, int idx2) -> bool {
        if (idx1 == std::numeric_limits<int>::max()) {
            return bool(pattern[i] < text[i + idx2]);
        } else {
            return bool(text[i + idx1] < pattern[i]);
        }
    });
    }
    std::vector<int> result(range.first, range.second);
    sort(result.begin(), result.end());

    return result;
}


int main() {
std::string text;
std::cin >> text;

TSuffixTree tree(text + '$');
TSuffixArray array(tree);

for(int count = 1; std::cin >> text; ++count) {
std::vector<int> result = array.Find(text);
//std::cout << count << ": ";
if(!result.empty()) {
    std::cout << count << ": ";
for(int i = 0; i < result.size(); ++i) {
    std::cout << result[i] + 1;
    if(i < result.size() - 1) {
        std::cout << ", ";
             }

        }
        std::cout << '\n';
    } //std::cout << '\n';
}

    return 0;
}