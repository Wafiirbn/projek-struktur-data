#include "LogAVL.hpp"
#include <algorithm>

using namespace std;

AVLNode::AVLNode(const LogEntry& e)
    : key(e.toEpoch()), entry(e), left(nullptr), right(nullptr), height(1) {}

LogAVL::LogAVL() : root(nullptr), size_(0) {}

LogAVL::~LogAVL() {
    destroyTree(root);
}

int LogAVL::getHeight(AVLNode* n) const { return n ? n->height : 0; }

int LogAVL::getBalance(AVLNode* n) const {
    return n ? getHeight(n->left) - getHeight(n->right) : 0;
}

void LogAVL::updateHeight(AVLNode* n) {
    if (n) n->height = 1 + max(getHeight(n->left), getHeight(n->right));
}

AVLNode* LogAVL::rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left  = T2;
    updateHeight(y);
    updateHeight(x);
    return x;
}

AVLNode* LogAVL::rotateLeft(AVLNode* x) {
    AVLNode* y  = x->right;
    AVLNode* T2 = y->left;
    y->left  = x;
    x->right = T2;
    updateHeight(x);
    updateHeight(y);
    return y;
}

AVLNode* LogAVL::balance(AVLNode* node) {
    updateHeight(node);
    int bf = getBalance(node);

    // Left-Left Case
    if (bf > 1 && getBalance(node->left) >= 0)
        return rotateRight(node);
    // Left-Right Case
    if (bf > 1 && getBalance(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    // Right-Right Case
    if (bf < -1 && getBalance(node->right) <= 0)
        return rotateLeft(node);
    // Right-Left Case
    if (bf < -1 && getBalance(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

AVLNode* LogAVL::insertNode(AVLNode* node, const LogEntry& e) {
    if (!node) return new AVLNode(e);
    if (e.toEpoch() <= node->key)
        node->left  = insertNode(node->left,  e);
    else
        node->right = insertNode(node->right, e);
    return balance(node);
}

void LogAVL::rangeQuery(AVLNode* node, long long ts, long long te, vector<LogEntry>& res) const {
    if (!node) return;
    if (node->key >= ts) rangeQuery(node->left,  ts, te, res);
    if (node->key >= ts && node->key <= te) res.push_back(node->entry);
    if (node->key <= te) rangeQuery(node->right, ts, te, res);
}

void LogAVL::collectGeq(AVLNode* node, long long cut, vector<LogEntry>& keep) const {
    if (!node) return;
    collectGeq(node->left,  cut, keep);
    if (node->key >= cut) keep.push_back(node->entry);
    collectGeq(node->right, cut, keep);
}

void LogAVL::destroyTree(AVLNode* node) {
    if (!node) return;
    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}

void LogAVL::inorder(AVLNode* node, vector<LogEntry>& res) const {
    if (!node) return;
    inorder(node->left, res);
    res.push_back(node->entry);
    inorder(node->right, res);
}

AVLNode* LogAVL::buildFromSorted(const vector<LogEntry>& logs, int start, int end) {
    if (start > end) return nullptr;
    int mid = start + (end - start) / 2;
    AVLNode* node = new AVLNode(logs[mid]);
    node->left  = buildFromSorted(logs, start, mid - 1);
    node->right = buildFromSorted(logs, mid + 1, end);
    updateHeight(node);
    return node;
}

void LogAVL::insert(const LogEntry& e) {
    root = insertNode(root, e);
    size_++;
}

vector<LogEntry> LogAVL::searchByTimeRange(const string& t_start, const string& t_end) const {
    LogEntry tmp; tmp.timestamp = t_start; long long ts = tmp.toEpoch();
    tmp.timestamp = t_end;                 long long te = tmp.toEpoch();
    vector<LogEntry> res;
    rangeQuery(root, ts, te, res);
    return res;
}

vector<LogEntry> LogAVL::searchByLevel(const string& level) const {
    vector<LogEntry> all;
    inorder(root, all);
    vector<LogEntry> res;
    for (auto& e : all)
        if (e.level == level) res.push_back(e);
    return res;
}

int LogAVL::deleteBefore(const string& cutoff) {
    LogEntry tmp; tmp.timestamp = cutoff;
    long long cut = tmp.toEpoch();
    vector<LogEntry> keep;
    collectGeq(root, cut, keep);
    int deleted = size_ - (int)keep.size();
    destroyTree(root);
    root  = buildFromSorted(keep, 0, (int)keep.size() - 1);
    size_ = (int)keep.size();
    return deleted;
}

vector<LogEntry> LogAVL::getAllSorted() const {
    vector<LogEntry> res;
    inorder(root, res);
    return res;
}

size_t LogAVL::estimateMemoryBytes() const {
    return (size_t)size_ * (sizeof(AVLNode) + 200);
}

int LogAVL::getTreeHeight() const { return getHeight(root); }

int LogAVL::size() const { return size_; }
