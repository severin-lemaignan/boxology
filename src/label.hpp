#ifndef LABEL_HPP
#define LABEL_HPP

#include <map>
#include <string>

enum class Label {
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
    MODEL,
    UI,
    OTHER
};

static const std::map<Label, std::string> LABEL_NAMES{
    {Label::PERCEPTION, "Perception"},
    {Label::ATTENTION, "Attention"},
    {Label::LANGUAGE, "Language"},
    {Label::DECISION_MAKING, "Decision making"},
    {Label::MEMORY, "Memory"},
    {Label::REASONING, "Reasoning"},
    {Label::SOCIOCOGNITION, "Socio-cog."},
    {Label::EMOTIONS, "Emotions"},
    {Label::LEARNING, "Learning"},
    {Label::ACTION, "Action"},
    {Label::MODEL, "Model/data"},
    {Label::UI, "UI"},
    {Label::OTHER, "Other"}};

static const std::map<Label, std::string> LABEL_COLORS{
    {Label::PERCEPTION, "#53399d"},
    {Label::ATTENTION, "#0476aa"},
    {Label::LANGUAGE, "#9d398b"},
    {Label::MEMORY, "#ffaa7f"},
    {Label::DECISION_MAKING, "#e1d620"},
    {Label::REASONING, "#c28e25"},
    {Label::SOCIOCOGNITION, "#399d40"},
    {Label::EMOTIONS, "#a11012"},
    {Label::LEARNING, "#2e13ff"},
    {Label::ACTION, "#75bdff"},
    {Label::MODEL, "#cca141"},
    {Label::UI, "#ddb241"},
    {Label::OTHER, "#1a1a1a"}};

Label get_label_by_name(const std::string& name);

#endif  // LABEL_HPP
