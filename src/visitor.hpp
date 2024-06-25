#ifndef VISITOR_HPP
#define VISITOR_HPP

#include <memory>
#include <string>

#include "architecture.hpp"
#include "node.hpp"

enum class EdgeType {
    TOPIC,
    SERVICE,
    ACTION,
    UNKNOWN
};

class Visitor {
   public:
    Visitor(const Architecture& architecture);

    std::string visit();

   protected:
    virtual void startUp(){};
    virtual void beginNodes(){};
    virtual void onNode(ConstNodePtr) = 0;
    virtual void endNodes(){};
    virtual void beginConnections(){};
    virtual void onConnection(std::shared_ptr<const Connection>) = 0;
    virtual void endConnections(){};
    virtual void tearDown(){};

    std::tuple<std::string, std::string> make_id(const std::string& name);

    /**
     * Returns the edge type of a given string, based on its prefix:
     * - "topic:" (or string starts with a '/') -> EdgeType::TOPIC
     * - "service:" -> EdgeType::SERVICE
     * - "action:" -> EdgeType::ACTION
     */
    EdgeType get_edge_type(const std::string& name);

    /**
     * Returns true if a given string starts with 'node:'
     */
    bool is_node_name(const std::string& name);

    std::string _content;

    const Architecture& architecture;
};

#endif  // VISITOR_HPP
