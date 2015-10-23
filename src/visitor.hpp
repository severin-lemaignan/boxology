#ifndef VISITOR_HPP
#define VISITOR_HPP

#include <memory>
#include <string>
#include "architecture.hpp"

class Visitor {
   public:
    Visitor(const Architecture& architecture);

    std::string visit();

   protected:
    virtual void startUp(){};
    virtual void beginNodes(){};
    virtual void onNode(std::shared_ptr<const Node>) = 0;
    virtual void endNodes(){};
    virtual void beginConnections(){};
    virtual void onConnection(std::shared_ptr<const Connection>) = 0;
    virtual void endConnections(){};
    virtual void tearDown(){};

    std::string _content;

    const Architecture& architecture;
};


#endif  // VISITOR_HPP
