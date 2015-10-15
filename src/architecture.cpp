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
    qWarning() << "Attempting to delete node " << QString::fromStdString(node->name()) <<  "...";
    _nodes.erase(node);
}


ConnectionPtr Architecture::createConnection(ConstNodePtr from,
                                             ConstPortPtr from_port,
                                             ConstNodePtr to, 
                                             ConstPortPtr to_port)
{
    auto connection = std::make_shared<Connection>();
    connection->from = {from, from_port};
    connection->to = {to, to_port};

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

