#ifndef ROSVISITOR_HPP
#define ROSVISITOR_HPP

#include <inja/inja.hpp>
#include <memory>
#include <sstream>
#include <string>

#include "architecture.hpp"  // Node
#include "cognitive_function.hpp"
#include "node.hpp"
#include "visitor.hpp"

class RosVisitor : public Visitor {
   public:
    RosVisitor(const Architecture& architecture, const std::string& ws_path);

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

    std::unique_ptr<inja::Environment> env_;
    nlohmann::json data_;
};

#endif
