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

string Visitor::make_id(const std::string& name) {
    string result;

    for (const char& c : name) {
        switch (c) {
            case ' ':
            case '.':
            case '-':
            case '\\':
            case '[':
            case ']':
            case '{':
            case '}':
            case '(':
            case ')':
            case '<':
            case '>':
            case '&':
            case ',':
            case '/':
            case '\'':
                break;
            default:
                result += tolower(c);
        }
    }

    return result;
}
