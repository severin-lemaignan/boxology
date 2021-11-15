#include "ros_visitor.hpp"

#include <algorithm>
#include <inja/inja.hpp>
#include <string>

#include "cognitive_function.hpp"
#include "node.hpp"

using namespace std;

void RosVisitor::startUp() {
    nlohmann::json data;
    data["workspace"] = ws_path;

    inja::render_to(std::cout, "Hello {{ workspace }}!",
                    data);  // Writes "Hello world!" to stream
}

void RosVisitor::tearDown() {}

void RosVisitor::beginNodes() {}

void RosVisitor::onNode(shared_ptr<const Node> node) {
    _nodes[node->cognitive_function()].push_back(node);
}

void RosVisitor::endNodes() {}

void RosVisitor::beginConnections() {}

void RosVisitor::onConnection(shared_ptr<const Connection> connection) {}

