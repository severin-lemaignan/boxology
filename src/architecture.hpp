#ifndef ARCHITECTURE_HPP
#define ARCHITECTURE_HPP

#include <memory>
#include <set>
#include <string>

#include "node.hpp"
#include "connection.hpp"

class Architecture
{
public:
    typedef std::set<NodePtr> Nodes;
    typedef std::set<ConstConnectionPtr> Connections;

    Architecture() {};

    NodePtr createNode() {
        auto node = std::make_shared<Node>();
        _nodes.insert(node);

        return node;
    }


    Nodes nodes() {return _nodes;}
    const Nodes nodes() const {return _nodes;}

    void addConnection(ConstConnectionPtr connection) {
        _connections.insert(connection);
    }

    Connections connections() {return _connections;}
    const Connections connections() const {return _connections;}

private:

    Nodes _nodes;
    Connections _connections;

    // remove nodes/connections marked for deletion
    void collect();

};

#endif // ARCHITECTURE_HPP
