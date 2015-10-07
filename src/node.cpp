/* See LICENSE file for copyright and license details. */

#include "graphicsnode.hpp"
#include "node.hpp"

using namespace std;

Node::Node():
        uuid(boost::uuids::random_generator()()),
        to_be_deleted(false)
{
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
