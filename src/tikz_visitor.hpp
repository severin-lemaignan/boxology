#ifndef TIKZVISITOR_HPP
#define TIKZVISITOR_HPP

#include <string>
#include <sstream>
#include <memory>

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
    std::string make_tex_id(const std::string& name);
    std::string tikz_unit(const double dim);

    std::stringstream ss;

    Architecture* _architecture;

    const double scale = 0.13;
    const double global_scale = 0.6;
};

#endif
