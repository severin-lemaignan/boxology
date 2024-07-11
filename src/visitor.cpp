#include "visitor.hpp"
#include "node.hpp"

#include <boost/algorithm/string.hpp> // for search and replace
#include <boost/uuid/uuid_io.hpp>
#include <regex>

using namespace std;

void capitalize(std::string &input) {
  std::string::iterator pos = input.begin();
  *pos = (char)toupper(*input.begin());
}

void camelCase(std::string &input) {
  bool isMakeUpper = false;
  std::string::iterator pos = input.begin();
  for (char c : input) {
    if (c == ' ') {
      isMakeUpper = true;
    } else if (isMakeUpper) {
      *pos++ = (char)toupper(c);
      isMakeUpper = false;
    } else {
      *pos++ = c;
    }
  }
  input.resize(pos - input.begin());
}

Visitor::Visitor(const Architecture &architecture)
    : architecture(architecture) {}

string Visitor::visit() {
  startUp();

  beginNodes();
  for (const auto &node : architecture.nodes()) {
    onNode(node);
  }
  endNodes();

  beginConnections();
  for (const auto &connection : architecture.connections()) {
    onConnection(connection);
  }
  endConnections();

  tearDown();

  return _content;
}

tuple<string, string> Visitor::get_id(const boost::uuids::uuid &id,
                                      const std::string &using_name) {

  if (_id_mappings.find(id) == _id_mappings.end()) {
    if (using_name != "") {
      _id_mappings[id] = make_id(using_name);
    } else {
      auto node = architecture.node(id);
      if (node == nullptr) {
        auto connection = architecture.connection(id);
        if (connection == nullptr) {
          std::stringstream ss;
          ss << id;
          throw runtime_error("No node or connection with id " + ss.str() +
                              "!");
        }
        _id_mappings[id] = make_id(connection->name);
      } else {
        _id_mappings[id] = make_id(node->name());
      }
    }
  }

  auto result = _id_mappings.at(id);

  string id_capitalized(result);
  camelCase(id_capitalized);
  capitalize(id_capitalized);

  return {result, id_capitalized};
}

string Visitor::make_id(const std::string &name, bool ignore_duplicates) {

  string result;

  string trimed_name(name);

  // remove leading / if present
  if (name.substr(0, 1) == "/") {
    trimed_name = name.substr(1);
  }

  // replace all '/*' with ''
  for (size_t pos = 0; (pos = trimed_name.find("/*", pos)) != string::npos;
       pos += 2) {
    trimed_name.replace(pos, 2, "");
  }

  for (const char &c : trimed_name) {

    /*
    Steps:

    - -, _ are kept as is
    - / is replaced by -
    - all other special characters are removed
    - all other characters are lowercased
    */
    switch (c) {
    case '.':
    case '`':
    case '+':
    case '?':
    case '\\':
    case '[':
    case ']':
    case '{':
    case '}':
    case '(':
    case ')':
    case '<':
    case '>':
    case '&':
    case ',':
    case ':':
    case '\'':
      break;
    case '/':
      result += '-';
      break;
    default:
      result += tolower(c);
    }
  }

  string id = regex_replace(result, regex("\\s"), "");

  if (id.empty()) {
    id = "anonymous";
  }

  if (!ignore_duplicates) {
    // check if the id is already in use. If so, append a number to it
    if (_used_ids.find(id) != _used_ids.end()) {
      int i = 1;
      string new_id;
      do {
        new_id = id + to_string(i++);
      } while (_used_ids.find(new_id) != _used_ids.end());
      id = new_id;
    }
  }
  _used_ids.insert(id);

  return id;
}

std::string Visitor::tex_escape(const std::string &text) {
  string result(text);
  trim(result);

  // special-case the backslash
  boost::replace_all(result, "\\", "\\textbackslash");

  map<string, string> tex_substitutions{
      {"{", "\\{"}, {"}", "\\}"},
      {"$", "\\$"}, {"&", "\\&"},
      {"#", "\\#"}, {"^", "\\textasciicircum{}"},
      {"_", "\\_"}, {"~", "\\textasciitilde{}"},
      {"%", "\\%"}};

  for (const auto &kv : tex_substitutions) {
    boost::replace_all(result, kv.first, kv.second);
  }

  // finish the backslash special-case
  boost::replace_all(result, "textbackslash", "textbackslash{}");

  return result;
}

EdgeType Visitor::get_edge_type(const std::string &name) const {
  if (name.find("service:") != string::npos) {
    return EdgeType::SERVICE;
  } else if (name.find("action:") != string::npos) {
    return EdgeType::ACTION;
  } else if (name.find("topic:") != string::npos || name.substr(0, 1) == "/") {
    return EdgeType::TOPIC;
  } else {
    return EdgeType::UNKNOWN;
  }
}

NodeType Visitor::get_node_type(ConstNodePtr node) const {

  if (node->label() == Label::MODEL) {
    return NodeType::MODEL;
  }
  if (node->label() == Label::HARDWARE) {
    return NodeType::HARDWARE;
  }

  auto name = node->name();
  if (name.find("plugin:") != string::npos) {
    return NodeType::PLUGIN;
  } else if (name.find("skill:") != string::npos) {
    return NodeType::SKILL;
  } else if (name.find("node:") != string::npos) {
    return NodeType::NODE;
  } else {
    return NodeType::UNKNOWN;
  }
}

bool Visitor::is_node_name(const std::string &name) {
  return name.substr(0, 5) == "node:";
}

bool Visitor::is_hardware(const std::string &name) {
  return name.substr(0, 3) == "hw:";
}
