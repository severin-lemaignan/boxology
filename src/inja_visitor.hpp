#ifndef INJA_VISITOR_HPP
#define INJA_VISITOR_HPP

#include <inja/inja.hpp>
#include <memory>
#include <sstream>
#include <string>

#include "architecture.hpp" // Node
#include "label.hpp"
#include "node.hpp"
#include "visitor.hpp"

class InjaVisitor : public Visitor {
public:
  InjaVisitor(const Architecture &architecture, const std::string &input_tpl,
              const std::string &output_path);

  bool ready() const { return env_ != nullptr; }

private:
  void startUp() override;
  void beginNodes() override;
  void onNode(std::shared_ptr<const Node>) override;
  void endNodes() override;
  void beginConnections() override;
  void onConnection(std::shared_ptr<const Connection>) override;
  void tearDown() override;

private:
  std::vector<ConstNodePtr> nodes_;

  std::string input_tpl;
  std::string output_path;

  std::unique_ptr<inja::Environment> env_;
  nlohmann::json data_;
};

#endif
