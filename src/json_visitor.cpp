#include <iostream>
#include <boost/uuid/uuid_io.hpp>

#include "json_visitor.hpp"
using namespace std;


void JsonVisitor::startUp() {
    ss << "{\"version\":\"0.1\"," << endl;
    ss << "\"nodes\":[ ";
}

void JsonVisitor::endNodes() {
    ss.seekp((long)(ss.tellp()) - 1);  // remove the last comma
    ss << endl
       << "], " << endl
       << "\"connections\":[ ";
}

void JsonVisitor::tearDown() {
    ss.seekp((long)(ss.tellp()) - 1);  // remove the last comma
    ss << endl
       << "]" << endl
       << "}" << endl;

    _content = ss.str();
}

void JsonVisitor::onNode(shared_ptr<const Node> node) {
    ss << endl
       << "{\"uuid\": \"" << node->uuid << "\", ";
    ss << "\"name\": \"" << node->name() << "\", ";
    ss << "\"ports\": [";
    for (const auto port : node->ports()) {
        ss << "{\"name\": \"" << port->name << "\",";
        ss << " \"direction\": \""
           << (port->direction == Port::Direction::IN ? "in" : "out") << "\"},";
    }
    ss.seekp((long)(ss.tellp()) - 1);  // remove the last comma
    ss << "]},";
}

void JsonVisitor::onConnection(shared_ptr<const Connection> connection) {
    ss << endl
       << "{\"uuid\": \"" << connection->uuid << "\", ";
    ss << "\"name\": \"" << connection->name << "\", ";
    ss << "\"from\": \"" << connection->from.node.lock()->uuid << ":"
       << connection->from.port.lock()->name << "\", ";
    ss << "\"to\": \"" << connection->to.node.lock()->uuid << ":"
       << connection->to.port.lock()->name << "\"},";
    // cout << "\"type\": \"" << connection->type << "\"" << endl;
}
