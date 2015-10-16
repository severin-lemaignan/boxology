#include <iostream>
#include <QDebug>
#include <json/json.h>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "architecture.hpp"

using namespace std;

NodePtr Architecture::createNode() {
    auto node = std::make_shared<Node>();
    _nodes.insert(node);

    return node;
}

NodePtr Architecture::createNode(boost::uuids::uuid uuid) {
    auto node = std::make_shared<Node>(uuid);
    _nodes.insert(node);

    return node;
}


void Architecture::addNode(NodePtr node) { _nodes.insert(node); }
void Architecture::removeNode(NodePtr node) { _nodes.erase(node); }

ConnectionPtr Architecture::createConnection(Socket from, Socket to) {
    auto connection = std::make_shared<Connection>();
    connection->from = from;
    connection->to = to;

    _connections.insert(connection);
    return connection;
}

void Architecture::addConnection(ConnectionPtr connection) {
    _connections.insert(connection);
}

void Architecture::removeConnection(Socket from, Socket to) {
    for (auto c : _connections) {
        if (c->to == to && c->from == from) {
            _connections.erase(c);
            break;
        }
    }
}


set<NodePtr> Architecture::update(const Json::Value& json,
                                  bool clearFirst, 
                                  bool recreateUUIDs) {

    set<NodePtr> newnodes;

    auto version = json.get("version", "0.1").asString();
    cout << "Reading the architecture (v." << version << ")..." << endl;

    set<boost::uuids::uuid> existing_uuids;

    if (clearFirst) {
        _connections.clear();
        _nodes.clear();
    }

    for (auto n : _nodes) {
        existing_uuids.insert(n->uuid);
    }
    for (auto c : _connections) {
        existing_uuids.insert(c->uuid);
    }


    auto nodes = json["nodes"];
    for (auto n : nodes) {
        auto uuid = boost::lexical_cast<boost::uuids::uuid>(n["uuid"].asString());

        NodePtr node;

        if (!recreateUUIDs) {
            if (existing_uuids.count(uuid)) {
                cerr << "Already existing UUID <" << uuid << ">! ";
                cerr << "Skipping this node." << endl;
                continue;
            }
            else {
                node = createNode(uuid);
            }
        }
        else {
                node = createNode();
        }

        cout << "Adding node <" << n["name"].asString() << ">" << endl;

        for (auto p : n["ports"]) {

            Port::Type type;
            if (p["type"].asString() == "latent") type = Port::Type::LATENT;
            else if (p["type"].asString() == "explicit") type = Port::Type::EXPLICIT;
            else type = Port::Type::OTHER;

            node->createPort({p["name"].asString(),
                              p["direction"].asString() == "in" ? Port::Direction::IN : Port::Direction::OUT,
                              type});
        }
        node->name(n["name"].asString());
        newnodes.insert(node);
    }

    return newnodes;
}
