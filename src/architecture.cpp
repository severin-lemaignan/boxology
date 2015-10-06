#include "architecture.hpp"

Architecture::Architecture()
{

}

void Architecture::collect() {

    for (auto node=_nodes.begin(); node!=_nodes.end();) {
        if ((*node)->to_be_deleted) node = _nodes.erase(node);
        else ++node;
    }

    for (auto connection=_connections.begin(); connection!=_connections.end();) {
        if ((*connection)->to_be_deleted) connection = _connections.erase(connection);
        else ++connection;
    }
}
