#ifndef ARCHITECTURE_HPP
#define ARCHITECTURE_HPP

#include<memory>
#include<set>
#include <string>

#include "node.hpp"
#include "connection.hpp"

class Architecture
{
public:
    typedef std::set<ConstNodePtr> Nodes;
    typedef std::set<ConstConnectionPtr> Connections;

    Architecture();

    void addNode(ConstNodePtr node) {
        _nodes.insert(node);
    }

    Nodes nodes() const {
        return _nodes;
    }

    void addConnection(ConstConnectionPtr connection) {
        _connections.insert(connection);
    }

    Connections connections() const {
        return _connections;
    }

private:
    Nodes _nodes;
    Connections _connections;

    // remove nodes/connections marked for deletion
    void collect();

};

#endif // ARCHITECTURE_HPP
