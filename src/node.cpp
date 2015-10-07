/* See LICENSE file for copyright and license details. */

#include "graphicsnode.hpp"
#include "node.hpp"

using namespace std;

Node::Node():
        uuid(boost::uuids::random_generator()()),
        to_be_deleted(false)
{
}

void Node::addPort(const Port port) {
    _ports.insert(make_shared<Port>(port));
    emit dirty(); // signal update
}

void Node::name(const std::string& name) {
    _name = name;
    emit dirty(); // signal update

}
