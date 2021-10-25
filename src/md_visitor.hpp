#ifndef MDVISITOR_HPP
#define MDVISITOR_HPP

#include <memory>
#include <sstream>
#include <string>

#include "architecture.hpp"  // Node
#include "cognitive_function.hpp"
#include "node.hpp"
#include "visitor.hpp"

class MdVisitor : public Visitor {
    using Visitor::Visitor;  // inheriting Visitor's ctor

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

    std::stringstream ss;
};

#endif
