/* See LICENSE file for copyright and license details. */

#include "graphicsnode.hpp"
#include "node.hpp"

#include <QDebug>

using namespace std;

Node::Node():
        uuid(boost::uuids::random_generator()())
{
}

Node::~Node()
{
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
    _ports.insert(portPtr);
    emit dirty(); // signal update
    return portPtr;
}

void Node::name(const std::string& name) {
    _name = name;
    emit dirty(); // signal update

}
