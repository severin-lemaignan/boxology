#ifndef COGNITIVE_FUNCTION_HPP
#define COGNITIVE_FUNCTION_HPP

#include <map>
#include <string>

enum class CognitiveFunction {
    PERCEPTION,
    ATTENTION,
    LANGUAGE,
    DECISION_MAKING,
    MEMORY,
    REASONING,
    SOCIOCOGNITION,
    EMOTIONS,
    LEARNING,
    ACTION,
    OTHER
};

static const std::map<CognitiveFunction, std::string> COGNITIVE_FUNCTION_NAMES{
    {CognitiveFunction::PERCEPTION, "Perception"},
    {CognitiveFunction::ATTENTION, "Attention"},
    {CognitiveFunction::LANGUAGE, "Language"},
    {CognitiveFunction::DECISION_MAKING, "Decision making"},
    {CognitiveFunction::MEMORY, "Memory"},
    {CognitiveFunction::REASONING, "Reasoning"},
    {CognitiveFunction::SOCIOCOGNITION, "Socio-cog."},
    {CognitiveFunction::EMOTIONS, "Emotions"},
    {CognitiveFunction::LEARNING, "Learning"},
    {CognitiveFunction::ACTION, "Action"},
    {CognitiveFunction::OTHER, "Other"}};

static const std::map<CognitiveFunction, std::string> COGNITIVE_FUNCTION_COLORS{
    {CognitiveFunction::PERCEPTION, "#53399d"},
    {CognitiveFunction::ATTENTION, "#0476aa"},
    {CognitiveFunction::LANGUAGE, "#9d398b"},
    {CognitiveFunction::MEMORY, "#ffaa7f"},
    {CognitiveFunction::DECISION_MAKING, "#e1d620"},
    {CognitiveFunction::REASONING, "#c28e25"},
    {CognitiveFunction::SOCIOCOGNITION, "#399d40"},
    {CognitiveFunction::EMOTIONS, "#a11012"},
    {CognitiveFunction::LEARNING, "#2e13ff"},
    {CognitiveFunction::ACTION, "#75bdff"},
    {CognitiveFunction::OTHER, "#1a1a1a"}};

CognitiveFunction get_cognitive_function_by_name(const std::string& name);

#endif  // COGNITIVE_FUNCTION_HPP
