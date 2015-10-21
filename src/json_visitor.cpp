#include <iostream>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include "json_visitor.hpp"

#include "cognitive_function.hpp"

using namespace std;

void JsonVisitor::startUp(const Architecture& architecture) {
    root["encoding_version"] = "0.1";
    root["name"] = architecture.name;
    root["version"] = architecture.version;
    root["description"] = architecture.description;
}

void JsonVisitor::tearDown() {
    Json::StyledWriter writer;
    _content = writer.write(root);
}

void JsonVisitor::onNode(shared_ptr<const Node> node) {
    Json::Value jnode;
    jnode["uuid"] = boost::lexical_cast<std::string>(node->uuid);
    jnode["name"] = node->name();
    jnode["cognitive_function"] =
        COGNITIVE_FUNCTION_NAMES.at(node->cognitive_function());
    jnode["position"].append(node->x());
    jnode["position"].append(node->y());

    for (const auto port : node->ports()) {
        Json::Value jport;
        jport["name"] = port->name;
        jport["direction"] =
            (port->direction == Port::Direction::IN ? "in" : "out");
        jnode["ports"].append(jport);
    }
    root["nodes"].append(jnode);
}

void JsonVisitor::onConnection(shared_ptr<const Connection> connection) {
    Json::Value jconn;
    jconn["uuid"] = boost::lexical_cast<std::string>(connection->uuid);
    jconn["name"] = connection->name;
    jconn["from"] =
        boost::lexical_cast<std::string>(connection->from.node.lock()->uuid) +
        ":" + connection->from.port.lock()->name;
    jconn["to"] =
        boost::lexical_cast<std::string>(connection->to.node.lock()->uuid) +
        ":" + connection->to.port.lock()->name;

    root["connections"].append(jconn);
}
