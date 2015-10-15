#include "visitor.hpp"

using namespace std;

Visitor::Visitor(const Architecture& architecture)
    : _architecture(architecture) {}

void Visitor::visit() {
    startUp();

    beginNodes();
    for (const auto node : _architecture.nodes()) {
        onNode(node);
    }
    endNodes();

    beginConnections();
    for (const auto connection : _architecture.connections()) {
        onConnection(connection);
    }
    endConnections();

    tearDown();
}
