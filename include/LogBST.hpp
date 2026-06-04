#ifndef LOG_BST_HPP
#define LOG_BST_HPP

#include "LogEntry.hpp"
#include <vector>
#include <string>

struct BSTNode {
    long long key;   // Unix epoch
    LogEntry entry;
    BSTNode* left;
    BSTNode* right;
    explicit BSTNode(const LogEntry& e);
};

class LogBST {
    BSTNode* root;
    int size_;

    // Helper functions
    void insertIter(const LogEntry& e);
    void rangeQuery(BSTNode* node, long long ts, long long te, std::vector<LogEntry>& res) const;
    void collectBefore(BSTNode* node, long long cut, std::vector<LogEntry>& keep) const;
    void destroyTree(BSTNode* node);
    void inorder(BSTNode* node, std::vector<LogEntry>& res) const;
    BSTNode* buildBalancedFromSorted(const std::vector<LogEntry>& sorted_logs, int start, int end);

public:
    LogBST();
    ~LogBST();

    // INSERT — O(log n) rata-rata jika teracak
    void insert(const LogEntry& e);

    // SEARCH by time range — O(log n + k)
    std::vector<LogEntry> searchByTimeRange(const std::string& t_start, const std::string& t_end) const;

    // SEARCH by level — O(n) traversal
    std::vector<LogEntry> searchByLevel(const std::string& level) const;

    // DELETE log lama — Rebuild tree seimbang O(n) agar BST tidak pincang/skewed
    int deleteBefore(const std::string& cutoff);

    std::vector<LogEntry> getAllSorted() const;

    int size() const;
};

#endif // LOG_BST_HPP
