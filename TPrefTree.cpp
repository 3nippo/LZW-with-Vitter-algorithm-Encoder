#include "TPrefTree.hpp"

TPrefTree::TPrefTree() {
    NextCode = 0;
    Points.push_back(TPrefTree::TPNode());
    for (int i = 0; i < 256; ++i) {
        std::string ins;
        ins += (unsigned char)i;
        Add(ins);
    }
    NextCode += 2;
    return;
}

TPrefTree::TPNode::TPNode() {
    Code = -1;
    NextPoints.resize(256, 0);
    return;
}

TPrefTree::TPNode::TPNode(const TPNode &right) {
    Code = right.Code;
    NextPoints = right.NextPoints;
    return;
}
TPrefTree::TPNode::TPNode(TPNode &&right) {
    Code = right.Code;
    NextPoints = std::move(right.NextPoints);
    return;
}
TPrefTree::TPNode& TPrefTree::TPNode::operator=(const TPNode &right) {
    Code = right.Code;
    NextPoints = right.NextPoints;
    return *this;
}
TPrefTree::TPNode& TPrefTree::TPNode::operator=(TPNode &&right){
    Code = right.Code;
    NextPoints = std::move(right.NextPoints);
    return *this;
}

std::size_t TPrefTree::TPNode::find(unsigned char c) {
    return NextPoints[c];
}

std::size_t TPrefTree::TPNode::add(unsigned char c, int point) {
    NextPoints[c] = point;
    return point;
}

void TPrefTree::Add(std::string &str) {
    std::size_t q = 0;
    for (unsigned char c : str) {
        auto it = Points[q].find(c);
        if (it == 0) {
            Points.push_back(TPrefTree::TPNode());
            it = Points[q].add(c, Points.size() - 1);
        }
        q = it;
    }
    Points[q].Code = NextCode++;
    return;
}

void TPrefTree::Add(std::size_t nodeNum, char c) {
    Points.push_back(TPrefTree::TPNode());
    Points[nodeNum].add(c, Points.size() - 1);
    Points[Points.size() - 1].Code = NextCode++;
    return;
}

SearchRes TPrefTree::Get(std::string &str) {
    std::size_t q = 0;
    for (unsigned char c : str) {
        auto it = Points[q].find(c);
        if (it == 0) {
            SearchRes ans;
            ans.Code = -1;
            ans.NodeNum = q;
            return ans;
        }
        q = it;
    }
    SearchRes ans;
    ans.Code = Points[q].Code;
    ans.NodeNum = q;
    return ans;
}

SearchRes TPrefTree::Get(std::size_t nodeNum, char c) {
    auto it = Points[nodeNum].find(c);
    if (it == 0) {
        SearchRes ans;
        ans.Code = -1;
        ans.NodeNum = nodeNum;
        return ans;
    }
    auto q = it;
    SearchRes ans;
    ans.Code = Points[q].Code;
    ans.NodeNum = q;
    return ans;
}

SearchRes TPrefTree::Get(std::size_t nodeNum) {
    SearchRes ans;
    ans.Code = Points[nodeNum].Code;
    ans.NodeNum = nodeNum;
    return ans;
}

void TPrefTree::Clear() {
    Points.clear();
    Points.push_back(TPrefTree::TPNode());
    NextCode = 0;
    for (int i = 0; i < 256; ++i) {
        std::string ins;
        ins += (unsigned char)i;
        Add(ins);
    }
    NextCode += 2;
    return;
}

int TPrefTree::Size() {
    return NextCode;
}
