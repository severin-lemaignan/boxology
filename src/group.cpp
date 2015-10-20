#include "group.hpp"

Group get_group_by_name(const std::string& name) {
    for (auto kv : GROUPNAME) {
        if (kv.second == name) return kv.first;
    }
    return Group::OTHER;
}

