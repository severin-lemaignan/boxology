#ifndef JSONVISITOR_HPP
#define JSONVISITOR_HPP

#include <string>
#include <sstream>
#include <memory>

#include "architecture.hpp"  // Node
#include "visitor.hpp"

class JsonVisitor : public Visitor<std::string> {
    using Visitor::Visitor;  // inheriting Visitor's ctor

    void startUp();
    void onNode(std::shared_ptr<const Node>);
    void endNodes();
    void onConnection(std::shared_ptr<const Connection>);
    void tearDown();

   private:
    std::stringstream ss;
};

#endif
