#ifndef TIKZVISITOR_HPP
#define TIKZVISITOR_HPP

#include <memory>
#include <sstream>
#include <string>

#include "architecture.hpp"  // Node
#include "visitor.hpp"

class TikzVisitor : public Visitor {
    using Visitor::Visitor;  // inheriting Visitor's ctor

   private:
    void startUp() override;
    void beginNodes() override;
    void onNode(std::shared_ptr<const Node>) override;
    void beginConnections() override;
    void onConnection(std::shared_ptr<const Connection>) override;
    void tearDown() override;

   private:
    std::string sanitize_tex(const std::string& text);
    std::string tikz_unit(const double dim);

    std::stringstream ss;

    Architecture* _architecture;

    const double pix2mm = 0.13;
};

#endif
