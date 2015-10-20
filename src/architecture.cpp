#include <iostream>
#include <json/json.h>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "architecture.hpp"

#include "group.hpp"

using namespace std;

NodePtr Architecture::createNode() {
    auto node = std::make_shared<Node>();
    _nodes.insert(node);

    return node;
}

NodePtr Architecture::createNode(const boost::uuids::uuid& uuid) {
    auto node = std::make_shared<Node>(uuid);
    _nodes.insert(node);

    return node;
}


void Architecture::addNode(NodePtr node) { _nodes.insert(node); }

void Architecture::removeNode(NodePtr node) {

    // first, delete all connections involving this node
    set<ConnectionPtr> to_remove;
    for (auto c : _connections) {
        if (   c->from.node.lock() == node
            || c->to.node.lock() == node) {
            to_remove.insert(c);
        }
    }
    for (auto c : to_remove) _connections.erase(c);

    _nodes.erase(node);
}

NodePtr Architecture::node(const boost::uuids::uuid& uuid) {
    for (auto n : _nodes) {
        if (n->uuid == uuid) return n;
    }
    return nullptr;
}

ConnectionPtr Architecture::createConnection(Socket from, Socket to) {
    auto connection = std::make_shared<Connection>();
    connection->from = from;
    connection->to = to;

    for (auto c : _connections) {
        if ((*c) == (*connection)) {
            return c;
        }
    }

    _connections.insert(connection);
    return connection;
}

ConnectionPtr Architecture::createConnection(const boost::uuids::uuid& uuid,
                                             Socket from, Socket to) {
    auto connection = std::make_shared<Connection>(uuid);
    connection->from = from;
    connection->to = to;

    for (auto c : _connections) {
        if ((*c) == (*connection)) {
            return c;
        }
    }

    _connections.insert(connection);
    return connection;
}

void Architecture::removeConnection(Socket from, Socket to) {
    for (auto c : _connections) {
        if (c->to == to && c->from == from) {
            _connections.erase(c);
            break;
        }
    }
}


Architecture::NodesAndConnections Architecture::update(const Json::Value& json,
                                                       bool clearFirst, 
                                                       bool recreateUUIDs) {

    set<NodePtr> newnodes;
    set<ConnectionPtr> newconnections;

    auto version = json.get("version", "<undefined>").asString();
    DEBUG("Reading the architecture (v." << version << ")..." << endl);

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


    //////////////////////////////////////////
    /////   NODES
    //////////////////////////////////////////
    for (auto n : json["nodes"]) {
        auto uuid = boost::lexical_cast<boost::uuids::uuid>(n["uuid"].asString());

        NodePtr node;

        if (!recreateUUIDs && existing_uuids.count(uuid)) {
            cerr << "Already existing UUID <" << uuid << ">! ";
            cerr << "Skipping this node." << endl;
            continue;
        }

        if(!recreateUUIDs){
            node = createNode(uuid);
        }
        else {
            node = createNode();
        }

        DEBUG("Adding node <" << n["name"].asString() << ">" << endl);

        node->group(get_group_by_name(n.get("group", "").asString()));

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
    
    //////////////////////////////////////////
    /////   CONNECTIONS
    //////////////////////////////////////////
    for (auto c : json["connections"]) {
        auto uuid = boost::lexical_cast<boost::uuids::uuid>(c["uuid"].asString());

        if (!recreateUUIDs && existing_uuids.count(uuid)) {
            cerr << "Already existing UUID <" << uuid << ">! ";
            cerr << "Skipping this connection." << endl;
            continue;
        }


        auto from_uuid_str = c["from"].asString().substr(0, 36);
        auto from_uuid = boost::lexical_cast<boost::uuids::uuid>(from_uuid_str);
        auto from = node(from_uuid);
        auto from_port = c["from"].asString().substr(37);

        auto to_uuid_str = c["to"].asString().substr(0, 36);
        auto to_uuid = boost::lexical_cast<boost::uuids::uuid>(to_uuid_str);
        auto to = node(to_uuid);
        auto to_port = c["to"].asString().substr(37);

        DEBUG("Creating connection between: " << from->name() << ":" << 
              from_port << " and " << to->name() << ":" << to_port << endl);

        ConnectionPtr connection;

        if (!recreateUUIDs) {
            connection = createConnection(uuid,
                                          {from, from->port(from_port)},
                                          {to, to->port(to_port)});
        }
        else {
            connection = createConnection({from, from->port(from_port)},
                                          {to, to->port(to_port)});
        }

        connection->name = c.get("name",Connection::ANONYMOUS).asString();

        newconnections.insert(connection);
    }

    return {newnodes, newconnections};
}
