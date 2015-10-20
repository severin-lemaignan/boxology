#ifndef JSONVISITOR_HPP
#define JSONVISITOR_HPP

#include <string>
#include <sstream>
#include <memory>

#include "architecture.hpp"  // Node
#include "visitor.hpp"

#include "json/json.h"

class JsonVisitor : public Visitor<std::string> {
    using Visitor::Visitor;  // inheriting Visitor's ctor

    void startUp(const Architecture& architecture);
    void onNode(std::shared_ptr<const Node>);
    void onConnection(std::shared_ptr<const Connection>);
    void tearDown();

   private:
    Json::Value root;
};

#endif
