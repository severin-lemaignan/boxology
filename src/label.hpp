#ifndef LABEL_HPP
#define LABEL_HPP

#include <map>
#include <string>

enum class Label {
    SOC_PERCEPTION,
    ENV_PERCEPTION,
    LANGUAGE,
    DECISION_MAKING,
    NAVIGATION,
    REASONING,
    MANIPULATION,
    EXPRESSIVENESS,
    LEARNING,
    MOTIONS,
    MODEL,
    UI,
    HARDWARE,
    OTHER
};

static const std::map<Label, std::string> LABEL_NAMES{
    {Label::SOC_PERCEPTION, "Social perception"},
    {Label::ENV_PERCEPTION, "Environment perception"},
    {Label::LANGUAGE, "Speech and Language"},
    {Label::DECISION_MAKING, "Decision making"},
    {Label::NAVIGATION, "Navigation"},
    {Label::REASONING, "Knowledge and Reasoning"},
    {Label::MANIPULATION, "Manipulation"},
    {Label::EXPRESSIVENESS, "Expressiveness"},
    {Label::LEARNING, "Learning"},
    {Label::MOTIONS, "Gesture, motions"},
    {Label::MODEL, "Model/data"},
    {Label::UI, "UI"},
    {Label::HARDWARE, "Hardware"},
    {Label::OTHER, "Other"},
};

static const std::map<Label, std::string> LABEL_COLORS{
    {Label::SOC_PERCEPTION, "#53399d"},
    {Label::ENV_PERCEPTION, "#0476aa"},
    {Label::LANGUAGE, "#9d398b"},
    {Label::NAVIGATION, "#ffaa7f"},
    {Label::DECISION_MAKING, "#e1d620"},
    {Label::REASONING, "#c28e25"},
    {Label::MANIPULATION, "#399d40"},
    {Label::EXPRESSIVENESS, "#a11012"},
    {Label::LEARNING, "#2e13ff"},
    {Label::MOTIONS, "#75bdff"},
    {Label::MODEL, "#cca141"},
    {Label::UI, "#ddb241"},
    {Label::HARDWARE, "#1a1a1a"},
    {Label::OTHER, "#1a1a1a"},
};

Label get_label_by_name(const std::string& name);

#endif  // LABEL_HPP
