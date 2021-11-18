#include "label.hpp"

Label get_label_by_name(const std::string& name) {
    for (auto kv : LABEL_NAMES) {
        if (kv.second == name) return kv.first;
    }
    return Label::OTHER;
}
