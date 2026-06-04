#ifndef LOG_AVL_HPP
#define LOG_AVL_HPP

#include "LogEntry.hpp"
#include <vector>
#include <string>
#include <algorithm>

struct AVLNode {
    long long key;   // Unix epoch
    LogEntry entry;
    AVLNode* left;
    AVLNode* right;
    int height;
    explicit AVLNode(const LogEntry& e);
};

class LogAVL {
    AVLNode* root;
    int size_;

    int getHeight(AVLNode* n) const;
    int getBalance(AVLNode* n) const;
    void updateHeight(AVLNode* n);

    // Rotasi
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    AVLNode* balance(AVLNode* node);

    AVLNode* insertNode(AVLNode* node, const LogEntry& e);
    void rangeQuery(AVLNode* node, long long ts, long long te, std::vector<LogEntry>& res) const;
    void collectGeq(AVLNode* node, long long cut, std::vector<LogEntry>& keep) const;
    void destroyTree(AVLNode* node);
    void inorder(AVLNode* node, std::vector<LogEntry>& res) const;
    AVLNode* buildFromSorted(const std::vector<LogEntry>& logs, int start, int end);

public:
    LogAVL();
    ~LogAVL();

    void insert(const LogEntry& e);
    std::vector<LogEntry> searchByTimeRange(const std::string& t_start, const std::string& t_end) const;
    std::vector<LogEntry> searchByLevel(const std::string& level) const;
    int deleteBefore(const std::string& cutoff);
    std::vector<LogEntry> getAllSorted() const;
    
    // Estimasi memori: per node (pointer kiri+kanan+parent) + LogEntry + height int
    size_t estimateMemoryBytes() const;
    int getTreeHeight() const;
    int size() const;
};

#endif // LOG_AVL_HPP
