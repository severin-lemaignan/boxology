/* See LICENSE file for copyright and license details. */

#include "node.hpp"

#include <QDebug>

using namespace std;

const map<Port::Type, std::string> Port::TYPENAME {{Type::EXPLICIT, "[->]"},
                                                   {Type::LATENT, "[~]"},
                                                   {Type::EVENT, "[!]"},
                                                   {Type::OTHER, ""}};
   
Node::Node() : Node(boost::uuids::random_generator()()) {}
Node::Node(boost::uuids::uuid uuid) : 
    uuid(uuid), 
    _x(0), _y(0),
    _cognitive_function(CognitiveFunction::OTHER)
{}

Node::~Node() {
    //qWarning() << "Node " << QString::fromStdString(_name) << " deleted!!";
}

/* Performs a deep-copy of the current Node, with however a different UUID
 */
NodePtr Node::duplicate() const {
    auto node = make_shared<Node>();
    node->name(_name + " (copy)");

    for (const auto p : _ports) {
        node->createPort(*p);
    }

    return node;
}

PortPtr Node::createPort(const Port port) {
    auto portPtr = make_shared<Port>(port);

    // check that we do not already have this port
    for (auto p : _ports) {
        if (p == portPtr) return nullptr;
    }

    _ports.insert(portPtr);
    emit dirty();  // signal update
    return portPtr;
}

void Node::remove_port(PortPtr port) {
    _ports.erase(port);

    emit dirty();
}

PortPtr Node::port(const string& name) {
    for (auto p : _ports) {
        if (p->name == name) return p;
    }
    return nullptr;
}

void Node::name(const std::string& name) {
    _name = name;
    emit dirty();
}

void Node::cognitive_function(CognitiveFunction cognitive_function) {
    _cognitive_function = cognitive_function;
    emit dirty();
}
