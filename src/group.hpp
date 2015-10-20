#ifndef GROUP_HPP
#define GROUP_HPP

#include <map>
#include <string>

enum class Group {PERCEPTION, MEMORY, ACTION, OTHER};

static const std::map<Group, std::string> GROUPNAME{{Group::PERCEPTION, "Perception"},
                                                    {Group::MEMORY, "Memory"},
                                                    {Group::ACTION, "Action"},
                                                    {Group::OTHER, "Other"}};

static const std::map<Group, std::string> GROUPCOLORS{{Group::PERCEPTION, "#55aa7f"},
                                                      {Group::MEMORY, "#ffaa7f"},
                                                      {Group::ACTION, "#75bdff"},
                                                      {Group::OTHER, "#1a1a1a"}};



Group get_group_by_name(const std::string& name);

#endif // GROUP_HPP
