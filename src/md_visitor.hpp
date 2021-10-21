#ifndef MDVISITOR_HPP
#define MDVISITOR_HPP

#include <string>
#include <sstream>
#include <memory>

#include "architecture.hpp"  // Node
#include "visitor.hpp"

class MdVisitor : public Visitor {
    using Visitor::Visitor;  // inheriting Visitor's ctor

private:
    void startUp() override;
    void beginNodes() override;
    void onNode(std::shared_ptr<const Node>) override;
    void beginConnections() override;
    void onConnection(std::shared_ptr<const Connection>) override;
    void tearDown() override;

private:

    std::stringstream ss;

};

#endif
