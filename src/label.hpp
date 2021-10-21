#ifndef LABEL_HPP
#define LABEL_HPP

#include <map>
#include <string>

// the order of values in this enum determine the order of icons in
// the UI + the order by which the nodes are listed in the markdown output
enum class Label {
    REASONING,        // CVUT
    MEMORY,           // UNITN
    PERCEPTION,       // INRIA
    LANGUAGE,         // BIU
    DECISION_MAKING,  // HWU
    SOCIOCOGNITION,   // ERM
    ATTENTION,        // PAL
    //    EMOTIONS,
    //    LEARNING,
    //    ACTION,
    OTHER
};

static const std::map<Label, std::string> LABEL_NAMES{
    {Label::PERCEPTION, "INRIA"},
    {Label::ATTENTION, "PAL"},
    {Label::LANGUAGE, "BIU"},
    {Label::DECISION_MAKING, "HWU"},
    {Label::MEMORY, "UNITN"},
    {Label::REASONING, "CVUT"},
    {Label::SOCIOCOGNITION, "ERM"},
    //    {Label::EMOTIONS, "Emotions"},
    //    {Label::LEARNING, "Learning"},
    //    {Label::ACTION, "Action"},
    {Label::OTHER, "Other"}};

static const std::map<Label, std::string> LABEL_COLORS{
    {Label::ATTENTION, "#53399d"},
    {Label::PERCEPTION, "#0476aa"},
    {Label::REASONING, "#9d398b"},
    {Label::DECISION_MAKING, "#ffaa7f"},
    {Label::SOCIOCOGNITION, "#e1d620"},
    {Label::LANGUAGE, "#c28e25"},
    {Label::MEMORY, "#399d40"},
    //    {Label::EMOTIONS, "#a11012"},
    //    {Label::LEARNING, "#2e13ff"},
    //    {Label::ACTION, "#75bdff"},
    {Label::OTHER, "#1a1a1a"}};

Label get_label_by_name(const std::string& name);

#endif  // LABEL_HPP
