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

    ConnectionPtr createConnection(ConstNodePtr from,
                                   ConstPortPtr from_port,
                                   ConstNodePtr to, 
                                   ConstPortPtr to_port);
    void addConnection(ConnectionPtr connection);
    void removeConnection(ConnectionPtr connection);

    Connections connections() {return _connections;}
    const Connections connections() const {return _connections;}

private:

    Nodes _nodes;
    Connections _connections;

};

#endif // ARCHITECTURE_HPP
