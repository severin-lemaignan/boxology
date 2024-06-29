#ifndef VISITOR_HPP
#define VISITOR_HPP

#include <memory>
#include <string>

#include "architecture.hpp"
#include "node.hpp"

#include <algorithm>
#include <cctype>
#include <locale>

//////// C++ trim functions, from https://stackoverflow.com/a/217605
// trim from start (in place)
inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

// trim from both ends (in place)
inline void trim(std::string &s) {
  rtrim(s);
  ltrim(s);
}
////////////////

enum class EdgeType { TOPIC, SERVICE, ACTION, UNKNOWN };

static const std::map<EdgeType, std::string> ROS_TYPE_NAMES{
    {EdgeType::TOPIC, "topic"},
    {EdgeType::SERVICE, "service"},
    {EdgeType::ACTION, "action"},
    {EdgeType::UNKNOWN, "unknown"},
};

enum class NodeType { NODE, HARDWARE, PLUGIN, UNKNOWN };

static const std::map<NodeType, std::string> NODE_TYPE_NAMES{
    {NodeType::NODE, "node"},
    {NodeType::HARDWARE, "hardware"},
    {NodeType::PLUGIN, "plugin"},
    {NodeType::UNKNOWN, "unknown"},
};

class Visitor {
public:
  Visitor(const Architecture &architecture);

  std::string visit();

protected:
  virtual void startUp(){};
  virtual void beginNodes(){};
  virtual void onNode(ConstNodePtr) = 0;
  virtual void endNodes(){};
  virtual void beginConnections(){};
  virtual void onConnection(std::shared_ptr<const Connection>) = 0;
  virtual void endConnections(){};
  virtual void tearDown(){};

  std::tuple<std::string, std::string> make_id(const std::string &name);
  std::string tex_escape(const std::string &name);

  /**
   * Returns the edge type of a given string, based on its prefix:
   * - "topic:" (or string starts with a '/') -> EdgeType::TOPIC
   * - "service:" -> EdgeType::SERVICE
   * - "action:" -> EdgeType::ACTION
   */
  EdgeType get_edge_type(const std::string &name);
  NodeType get_node_type(const std::string &name);

  /**
   * Returns true if a given string starts with 'node:'
   */
  bool is_node_name(const std::string &name);

  /**
   * Returns true if a given string starts with 'hw:' (ie, represent a
   * hardware resource)
   */
  bool is_hardware(const std::string &name);

  std::string _content;

  const Architecture &architecture;
};

#endif // VISITOR_HPP
