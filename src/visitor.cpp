#include "visitor.hpp"

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
  for (const auto node : architecture.nodes()) {
    onNode(node);
  }
  endNodes();

  beginConnections();
  for (const auto connection : architecture.connections()) {
    onConnection(connection);
  }
  endConnections();

  tearDown();

  return _content;
}

tuple<string, string> Visitor::make_id(const std::string &name) {
  string result;

  for (const char &c : name) {
    switch (c) {
    case '.':
    case '`':
    case '+':
    case '?':
    case '-':
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
    case '/':
    case ':':
    case '\'':
      break;
    default:
      result += tolower(c);
    }
  }

  string id = regex_replace(result, regex("\\s"), "");

  string id_capitalized(result);
  camelCase(id_capitalized);
  capitalize(id_capitalized);

  return {id, id_capitalized};
}

EdgeType Visitor::get_edge_type(const std::string &name) {
    if (name.find("service:") != string::npos) {
        return EdgeType::SERVICE;
    } else if (name.find("action:") != string::npos) {
        return EdgeType::ACTION;
    }
    else if (name.find("topic:") != string::npos || name.substr(0,1) == "/"){
        return EdgeType::TOPIC;
    } else {
        return EdgeType::UNKNOWN;
    }
}

bool Visitor::is_node_name(const std::string &name) {
    return name.substr(0,5) == "node:";
}

bool Visitor::is_hardware(const std::string &name) {
    return name.substr(0,3) == "hw:";
}
