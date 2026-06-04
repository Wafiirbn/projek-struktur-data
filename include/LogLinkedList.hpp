#ifndef LOG_LINKED_LIST_HPP
#define LOG_LINKED_LIST_HPP

#include "LogEntry.hpp"
#include <vector>
#include <string>
#include <unordered_map>

struct LLNode {
    LogEntry entry;
    LLNode* next;
    explicit LLNode(const LogEntry& e);
};

class LogLinkedList {
    LLNode* head;
    LLNode* tail;
    int size_;

public:
    LogLinkedList();
    ~LogLinkedList();

    // INSERT — O(1)
    void insert(const LogEntry& e);

    // SEARCH by level — O(n)
    std::vector<LogEntry> searchByLevel(const std::string& level) const;

    // SEARCH by module — O(n)
    std::vector<LogEntry> searchByModule(const std::string& mod) const;

    // SEARCH by time range — O(n)
    std::vector<LogEntry> searchByTimeRange(const std::string& t_start, const std::string& t_end) const;

    // DELETE log lama (sebelum cutoff) — O(n)
    int deleteBefore(const std::string& cutoff);

    // STATISTIK per level — O(n)
    std::unordered_map<std::string, int> statistics() const;

    // Estimasi memori
    size_t estimateMemoryBytes() const;

    int size() const;

    void printAll(int limit = 10) const;
};

#endif // LOG_LINKED_LIST_HPP
