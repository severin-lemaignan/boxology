#include "cognitive_function.hpp"

CognitiveFunction get_cognitive_function_by_name(const std::string& name) {
    for (auto kv : COGNITIVE_FUNCTION_NAMES) {
        if (kv.second == name) return kv.first;
    }
    return CognitiveFunction::OTHER;
}
