#ifndef ROSVISITOR_HPP
#define ROSVISITOR_HPP

#include <memory>
#include <sstream>
#include <string>

#include "architecture.hpp"  // Node
#include "cognitive_function.hpp"
#include "node.hpp"
#include "visitor.hpp"

class RosVisitor : public Visitor {
    using Visitor::Visitor;  // inheriting Visitor's ctor

   public:
    void setWorkspace(const std::string& path) { ws_path = path; }

   private:
    void startUp() override;
    void beginNodes() override;
    void onNode(std::shared_ptr<const Node>) override;
    void endNodes() override;
    void beginConnections() override;
    void onConnection(std::shared_ptr<const Connection>) override;
    void tearDown() override;

   private:
    std::map<CognitiveFunction, std::vector<ConstNodePtr>> _nodes;

    std::string ws_path;
};

#endif
