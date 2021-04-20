#ifndef _NTREE_H_
#define _NTREE_H_

#include <unistd.h>

#include <iostream>
#include <memory>
#include <unordered_set>
#include <utility>

const std::pair<int, pid_t> BAD_RES = {-1, -1};

class n_tree_node {
private:
    std::pair<int, pid_t> id_ = BAD_RES;
    std::unordered_set<std::shared_ptr<n_tree_node>> children_;
public:
    n_tree_node(std::pair<int, pid_t> id = BAD_RES);
    bool add_to(int parrent_id, std::pair<int, pid_t> new_id);
    bool remove(int id);
    bool find(int id) const;
    std::pair<int, pid_t>& get(int id);
    std::unordered_set<pid_t> get_all_second() const;
    std::unordered_set<int> get_all_first() const;
    std::pair<int, pid_t> id() const;
};

class n_tree {
private:
    std::shared_ptr<n_tree_node> head_;
public:
    bool add_to(int parrent_id, std::pair<int, pid_t> new_id);
    bool remove(int id);
    bool find(int id) const;
    std::pair<int, pid_t>& get(int id);
    std::unordered_set<pid_t> get_all_second() const;
    std::unordered_set<int> get_all_first() const;
};

#endif
