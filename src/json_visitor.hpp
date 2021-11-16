#ifndef JSONVISITOR_HPP
#define JSONVISITOR_HPP

#include <memory>
#include <sstream>
#include <string>

#include "architecture.hpp"  // Node
#include "json/json.h"
#include "visitor.hpp"

class JsonVisitor : public Visitor {
   public:
    JsonVisitor(const Architecture& architecture) : Visitor(architecture) {}

    void startUp() override;
    void onNode(std::shared_ptr<const Node>) override;
    void onConnection(std::shared_ptr<const Connection>) override;
    void tearDown() override;

    Json::Value getJson() const { return root; }

   private:
    Json::Value root;
    Json::Value arch;
};

#endif
