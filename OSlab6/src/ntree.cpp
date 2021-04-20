#include "ntree.h"

#include <algorithm>
#include <exception>

n_tree_node::n_tree_node(std::pair<int, pid_t> id) : id_(id) {}

bool n_tree_node::add_to(int parrent_id, std::pair<int, pid_t> new_id) {
    if (id_.first == parrent_id) {
        children_.insert(std::make_shared<n_tree_node>(new_id));
        return true;
    } 
    else {
        bool is_ok = false;
        for (const auto& ch_ptr : children_) {
            is_ok = is_ok || ch_ptr->add_to(parrent_id, new_id);
            if (is_ok) {
                break;
            }
        }
        return is_ok;
    }
}

bool n_tree_node::remove(int id) {
    auto it = std::find_if(children_.begin(), children_.end(), [id](const auto& ptr) { 
                                                                                         return ptr->id_.first == id; 
                                                                                     });
    if (it != children_.end()) {
        children_.erase(it);
        return true;
    } 
    else {
        bool is_ok = false;
        for (const auto& ch_ptr : children_) {
            is_ok = is_ok || ch_ptr->remove(id);
            if (is_ok) {
                break;
            }
        }
        return is_ok;
    }
}

bool n_tree_node::find(int id) const {
    if (id_.first == id) {
        return true;
    } 
    else {
        bool is_ok = false;
        for (const auto& ch_ptr : children_) {
            is_ok = is_ok || ch_ptr->find(id);
            if (is_ok) {
                return true;
            }
        }
        return is_ok;
    }
}

std::pair<int, pid_t>& n_tree_node::get(int id) {
    if (id_.first == id) {
        return id_;
    } 
    else {
        for (const auto& ch_ptr : children_) {
            if (ch_ptr->find(id)) {
                return ch_ptr->get(id);
            }
        }
        throw std::runtime_error("can't get value");
    }
}

std::unordered_set<pid_t> n_tree_node::get_all_second() const {
    std::unordered_set<pid_t> res;
    for (const auto& ptr : children_) {
        res.insert(ptr->id().second);
        res.merge(ptr->get_all_second());
    }
    return res;
}

std::unordered_set<pid_t> n_tree_node::get_all_first() const {
    std::unordered_set<pid_t> res;
    for (const auto& ptr : children_) {
        res.insert(ptr->id().first);
        res.merge(ptr->get_all_first());
    }
    return res;
}

std::pair<int, pid_t> n_tree_node::id() const {
    return id_;
}

bool n_tree::add_to(int parrent_id, std::pair<int, pid_t> new_id) {
    if (!head_) {
        head_ = std::make_shared<n_tree_node>(new_id);
        return true;
    }
    if (find(new_id.first)) {
        return false;
    }
    return head_->add_to(parrent_id, new_id);
}

bool n_tree::remove(int id) {
    if (head_) {
        if (id == head_->id().first) {
            head_ = nullptr;
            return true;
        }
        return head_->remove(id);
    }
    return false;
}

bool n_tree::find(int id) const {
    if (head_) {
        return head_->find(id);
    }
    return false;
}

std::pair<int, pid_t>& n_tree::get(int id) {
    if (head_) {
        return head_->get(id);
    }
    throw std::runtime_error("can't get value");
}

std::unordered_set<pid_t> n_tree::get_all_second() const {
    if (head_) {
        std::unordered_set<pid_t> res;
        res.insert(head_->id().second);
        res.merge(head_->get_all_second());
        return res;
    }
    return {};
}

std::unordered_set<pid_t> n_tree::get_all_first() const {
    if (head_) {
        std::unordered_set<pid_t> res;
        res.insert(head_->id().first);
        res.merge(head_->get_all_first());
        return res;
    }
    return {};
}
