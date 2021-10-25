#include "md_visitor.hpp"

#include <algorithm>
#include <string>

#include "cognitive_function.hpp"
#include "node.hpp"

using namespace std;

void MdVisitor::startUp() {
    ss << "# " << architecture.name << endl;
    ss << endl;
    ss << "Version **" << architecture.version << "**" << endl;
    ss << endl;
    ss << architecture.description << endl;
    ss << endl;
}

void MdVisitor::tearDown() {
    ss << endl;

    _content = ss.str();
}

void MdVisitor::beginNodes() { ss << "## Nodes" << endl; }

void MdVisitor::onNode(shared_ptr<const Node> node) {
    _nodes[node->cognitive_function()].push_back(node);
}

void MdVisitor::endNodes() {
    for (const auto& kv : _nodes) {
        for (auto node : kv.second) {
            ss << "### " << node->name() << endl << endl;
            ss << "Team: **"
               << COGNITIVE_FUNCTION_NAMES.at(node->cognitive_function())
               << "**" << endl
               << endl;

            ss << "- I/O:" << endl;

            for (auto p : node->ports()) {
                bool isInput = (p->direction == Port::Direction::IN);

                ss << "  - `" << p->name << "` (**" << (isInput ? "IN" : "OUT")
                   << "**)" << endl;
            }
            ss << endl;
        }
    }
}

void MdVisitor::beginConnections() {
    ss << endl;
    ss << "## Links" << endl;
}

void MdVisitor::onConnection(shared_ptr<const Connection> connection) {
    auto name = connection->name;
    auto from = connection->from.node.lock();
    auto to = connection->to.node.lock();

    // if (connection->name == "anonymous") {
    //    ss << "        \\path (" << make_tex_id(from->name())
    //       << ") "
    //          "edge [->, out=0, in=180] "
    //          "("
    //       << make_tex_id(to->name()) << ");" << endl;
    //} else {
    //    ss << "        \\path (" << make_tex_id(from->name())
    //       << ") "
    //          "edge [->, out=0, in=180] node[label] {"
    //       << sanitize_tex(connection->name)
    //       << "}"
    //          "("
    //       << make_tex_id(to->name()) << ");" << endl;
    //}
}

