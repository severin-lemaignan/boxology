#include "json_visitor.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>

#include "cognitive_function.hpp"
#include "json/json.h"

using namespace std;

void JsonVisitor::startUp() {
    root["root"] = boost::lexical_cast<std::string>(architecture.uuid);

    arch["name"] = architecture.name;
    arch["version"] = architecture.version;
    arch["description"] = architecture.description;
    arch["uuid"] = boost::lexical_cast<std::string>(architecture.uuid);
}

void JsonVisitor::tearDown() {
    root["architectures"].append(arch);

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
    jnode["size"].append(node->width());
    jnode["size"].append(node->height());

    if (node->sub_architecture) {
        jnode["sub_architecture"] =
            boost::lexical_cast<std::string>(node->sub_architecture->uuid);

        JsonVisitor json(*node->sub_architecture);
        json.visit();

        // flatten the hierarchy of architectures
        for (size_t idx = 0; idx < json.getJson()["architectures"].size();
             idx++) {
            root["architectures"].append(
                json.getJson()["architectures"][Json::ArrayIndex(idx)]);
        }
    }

    for (const auto port : node->ports()) {
        Json::Value jport;
        jport["name"] = port->name;
        jport["direction"] =
            (port->direction == Port::Direction::IN ? "in" : "out");
        jnode["ports"].append(jport);
    }
    arch["nodes"].append(jnode);
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

    arch["connections"].append(jconn);
}
