#include "visitor.hpp"

using namespace std;

Visitor::Visitor(const Architecture& architecture)
    : architecture(architecture) {}

string Visitor::visit() {
    startUp();

    beginNodes();
    for (const auto node : architecture.nodes()) {
        onNode(node);
    }
    endNodes();

    beginConnections();
    for (const auto connection : architecture.connections()) {
        onConnection(connection);
    }
    endConnections();

    tearDown();

    return _content;
}
