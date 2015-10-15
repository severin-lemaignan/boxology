#include <QDebug>

#include "architecture.hpp"

NodePtr Architecture::createNode()
{
    auto node = std::make_shared<Node>();
    _nodes.insert(node);

    return node;
}

void Architecture::addNode(NodePtr node)
{
    _nodes.insert(node);
}
void Architecture::removeNode(NodePtr node)
{
    _nodes.erase(node);
}


ConnectionPtr Architecture::createConnection(Socket from, Socket to)
{
    auto connection = std::make_shared<Connection>();
    connection->from = from;
    connection->to = to;

    _connections.insert(connection);
    return connection;
}

void Architecture::addConnection(ConnectionPtr connection)
{
    _connections.insert(connection);
}
void Architecture::removeConnection(ConnectionPtr connection)
{
    _connections.erase(connection);
}

