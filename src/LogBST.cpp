#include "LogBST.hpp"

BSTNode::BSTNode(const LogEntry& e)
    : key(e.toEpoch()), entry(e), left(nullptr), right(nullptr) {}

LogBST::LogBST() : root(nullptr), size_(0) {}

LogBST::~LogBST() {
    destroyTree(root);
}

void LogBST::insertIter(const LogEntry& e) {
    BSTNode* node = new BSTNode(e);
    if (!root) {
        root = node;
        size_++;
        return;
    }
    BSTNode* cur = root;
    while (true) {
        if (node->key <= cur->key) {
            if (!cur->left) {
                cur->left = node;
                break;
            } else {
                cur = cur->left;
            }
        } else {
            if (!cur->right) {
                cur->right = node;
                break;
            } else {
                cur = cur->right;
            }
        }
    }
    size_++;
}

void LogBST::rangeQuery(BSTNode* node, long long ts, long long te, std::vector<LogEntry>& res) const {
    if (!node) return;
    if (node->key >= ts) rangeQuery(node->left, ts, te, res);
    if (node->key >= ts && node->key <= te) res.push_back(node->entry);
    if (node->key <= te) rangeQuery(node->right, ts, te, res);
}

void LogBST::collectBefore(BSTNode* node, long long cut, std::vector<LogEntry>& keep) const {
    if (!node) return;
    collectBefore(node->left, cut, keep);
    if (node->key >= cut) keep.push_back(node->entry);
    collectBefore(node->right, cut, keep);
}

void LogBST::destroyTree(BSTNode* node) {
    if (!node) return;
    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}

void LogBST::inorder(BSTNode* node, std::vector<LogEntry>& res) const {
    if (!node) return;
    inorder(node->left, res);
    res.push_back(node->entry);
    inorder(node->right, res);
}

BSTNode* LogBST::buildBalancedFromSorted(const std::vector<LogEntry>& sorted_logs, int start, int end) {
    if (start > end) return nullptr;
    int mid = start + (end - start) / 2;
    BSTNode* node = new BSTNode(sorted_logs[mid]);
    node->left = buildBalancedFromSorted(sorted_logs, start, mid - 1);
    node->right = buildBalancedFromSorted(sorted_logs, mid + 1, end);
    return node;
}

void LogBST::insert(const LogEntry& e) {
    insertIter(e);
}

std::vector<LogEntry> LogBST::searchByTimeRange(const std::string& t_start, const std::string& t_end) const {
    LogEntry tmp;
    tmp.timestamp = t_start;
    long long ts = tmp.toEpoch();
    tmp.timestamp = t_end;
    long long te = tmp.toEpoch();
    std::vector<LogEntry> res;
    rangeQuery(root, ts, te, res);
    return res;
}

std::vector<LogEntry> LogBST::searchByLevel(const std::string& level) const {
    std::vector<LogEntry> all;
    inorder(root, all);
    std::vector<LogEntry> res;
    for (auto& e : all) {
        if (e.level == level) {
            res.push_back(e);
        }
    }
    return res;
}

int LogBST::deleteBefore(const std::string& cutoff) {
    LogEntry tmp;
    tmp.timestamp = cutoff;
    long long cut = tmp.toEpoch();
    std::vector<LogEntry> keep;
    collectBefore(root, cut, keep);
    int deleted = size_ - (int)keep.size();
    destroyTree(root);
    
    // Membangun ulang secara seimbang (Balanced Tree Reconstruction)
    root = buildBalancedFromSorted(keep, 0, (int)keep.size() - 1);
    size_ = (int)keep.size();
    return deleted;
}

std::vector<LogEntry> LogBST::getAllSorted() const {
    std::vector<LogEntry> res;
    inorder(root, res);
    return res;
}

int LogBST::size() const {
    return size_;
}
