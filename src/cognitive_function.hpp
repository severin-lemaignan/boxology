#ifndef COGNITIVE_FUNCTION_HPP
#define COGNITIVE_FUNCTION_HPP

#include <map>
#include <string>

enum class CognitiveFunction {PERCEPTION, 
                              LEARNING, 
                              DECISION_MAKING, 
                              MEMORY, 
                              REASONING, 
                              LANGUAGE, 
                              ATTENTION, 
                              ACTION, 
                              OTHER};

static const std::map<CognitiveFunction, std::string> COGNITIVE_FUNCTION_NAMES{
                        {CognitiveFunction::PERCEPTION, "Perception"},
                        {CognitiveFunction::LEARNING, "Learning"},
                        {CognitiveFunction::MEMORY, "Memory"},
                        {CognitiveFunction::DECISION_MAKING, "Decision making"},
                        {CognitiveFunction::REASONING, "Reasoning"},
                        {CognitiveFunction::LANGUAGE, "Language"},
                        {CognitiveFunction::ATTENTION, "Attention"},
                        {CognitiveFunction::ACTION, "Action"},
                        {CognitiveFunction::OTHER, "Other"}};

static const std::map<CognitiveFunction, std::string> COGNITIVE_FUNCTION_COLORS{
                        {CognitiveFunction::PERCEPTION, "#ff0000"},
                        {CognitiveFunction::LEARNING, "#9d398b"},
                        {CognitiveFunction::MEMORY, "#ffaa7f"},
                        {CognitiveFunction::DECISION_MAKING, "#e1d620"},
                        {CognitiveFunction::REASONING, "#399d40"},
                        {CognitiveFunction::LANGUAGE, "#53399d"},
                        {CognitiveFunction::ATTENTION, "#9d3939"},
                        {CognitiveFunction::ACTION, "#75bdff"},
                        {CognitiveFunction::OTHER, "#1a1a1a"}};



CognitiveFunction get_cognitive_function_by_name(const std::string& name);

#endif // COGNITIVE_FUNCTION_HPP
