/* See LICENSE file for copyright and license details. */

#include "node.hpp"

#include <QDebug>

using namespace std;

InterfaceType get_interface_type_by_name(const std::string &name) {
  for (auto kv : INTERFACE_TYPE_NAMES) {
    if (kv.second == name)
      return kv.first;
  }
  return InterfaceType::UNKNOWN;
}

NodeType get_node_type_by_name(const std::string &name) {
  for (auto kv : NODE_TYPE_NAMES) {
    if (kv.second == name)
      return kv.first;
  }
  return NodeType::UNKNOWN;
}

Node::Node() : Node(boost::uuids::random_generator()()) {}
Node::Node(boost::uuids::uuid uuid)
    : uuid(uuid), _x(0), _y(0), _label(Label::OTHER) {}

Node::~Node() {
  // qWarning() << "Node " << QString::fromStdString(_name) << " deleted!!";
}

/* Performs a deep-copy of the current Node, with however a new UUID
 */
NodePtr Node::duplicate() const {
  auto node = make_shared<Node>();
  node->name(_name + " (copy)");
  node->label(_label);

  for (const auto &p : _ports) {
    node->createPort(*p);
  }

  return node;
}

PortPtr Node::createPort(const Port port) {
  auto portPtr = make_shared<Port>(port);

  // check that we do not already have this port
  for (const auto &p : _ports) {
    if (p == portPtr)
      return nullptr;
  }

  _ports.insert(portPtr);
  emit dirty(); // signal update
  return portPtr;
}

void Node::removePort(PortPtr port) {
  _ports.erase(port);

  emit dirty();
}

PortPtr Node::port(const string &name) {
  for (const auto &p : _ports) {
    if (p->name == name)
      return p;
  }
  return nullptr;
}

void Node::name(const std::string &name) {
  _name = name;
  emit dirty();
}

void Node::label(Label label) {
  _label = label;
  emit dirty();
}

void Node::type(NodeType type) {
  _type = type;
  emit dirty();
}
