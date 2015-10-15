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

    NodePtr createNode();
    void addNode(NodePtr node);
    void removeNode(NodePtr node);

    Nodes nodes() {return _nodes;}
    const Nodes nodes() const {return _nodes;}

    ConnectionPtr createConnection(Socket from, Socket to);
    void addConnection(ConnectionPtr connection);
    void removeConnection(Socket from, Socket to);

    Connections connections() {return _connections;}
    const Connections connections() const {return _connections;}

private:

    Nodes _nodes;
    Connections _connections;

};

#endif // ARCHITECTURE_HPP
