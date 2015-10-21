#include "visitor.hpp"

using namespace std;

template <class T>
Visitor<T>::Visitor(const Architecture& architecture)
    : _architecture(architecture) {}

template <class T>
T Visitor<T>::visit() {
    startUp(_architecture);

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

    return _content;
}
