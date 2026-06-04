#include "LogLinkedList.hpp"
#include <iostream>

LLNode::LLNode(const LogEntry& e) : entry(e), next(nullptr) {}

LogLinkedList::LogLinkedList() : head(nullptr), tail(nullptr), size_(0) {}

LogLinkedList::~LogLinkedList() {
    LLNode* cur = head;
    while (cur) {
        LLNode* tmp = cur->next;
        delete cur;
        cur = tmp;
    }
}

void LogLinkedList::insert(const LogEntry& e) {
    LLNode* node = new LLNode(e);
    if (!tail) {
        head = tail = node;
    } else {
        tail->next = node;
        tail = node;
    }
    size_++;
}

std::vector<LogEntry> LogLinkedList::searchByLevel(const std::string& level) const {
    std::vector<LogEntry> res;
    for (LLNode* cur = head; cur; cur = cur->next) {
        if (cur->entry.level == level) {
            res.push_back(cur->entry);
        }
    }
    return res;
}

std::vector<LogEntry> LogLinkedList::searchByModule(const std::string& mod) const {
    std::vector<LogEntry> res;
    for (LLNode* cur = head; cur; cur = cur->next) {
        if (cur->entry.module == mod) {
            res.push_back(cur->entry);
        }
    }
    return res;
}

std::vector<LogEntry> LogLinkedList::searchByTimeRange(const std::string& t_start, const std::string& t_end) const {
    LogEntry tmp;
    tmp.timestamp = t_start;
    long long ts = tmp.toEpoch();
    tmp.timestamp = t_end;
    long long te = tmp.toEpoch();
    std::vector<LogEntry> res;
    for (LLNode* cur = head; cur; cur = cur->next) {
        long long t = cur->entry.toEpoch();
        if (t >= ts && t <= te) {
            res.push_back(cur->entry);
        }
    }
    return res;
}

int LogLinkedList::deleteBefore(const std::string& cutoff) {
    LogEntry tmp;
    tmp.timestamp = cutoff;
    long long cut = tmp.toEpoch();
    int deleted = 0;
    LLNode* prev = nullptr;
    LLNode* cur  = head;
    while (cur) {
        if (cur->entry.toEpoch() < cut) {
            LLNode* del = cur;
            if (!prev) {
                head = cur->next;
            } else {
                prev->next = cur->next;
            }
            if (del == tail) {
                tail = prev;
            }
            cur = cur->next;
            delete del;
            deleted++;
            size_--;
        } else {
            prev = cur;
            cur  = cur->next;
        }
    }
    return deleted;
}

std::unordered_map<std::string, int> LogLinkedList::statistics() const {
    std::unordered_map<std::string, int> stats;
    for (LLNode* cur = head; cur; cur = cur->next) {
        stats[cur->entry.level]++;
    }
    return stats;
}

int LogLinkedList::size() const {
    return size_;
}

void LogLinkedList::printAll(int limit) const {
    int cnt = 0;
    for (LLNode* cur = head; cur && cnt < limit; cur = cur->next, cnt++) {
        cur->entry.print();
    }
    if (size_ > limit) {
        std::cout << "  ... (" << size_ - limit << " entri lainnya)\n";
    }
}
