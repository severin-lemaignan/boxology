#ifndef VISITOR_HPP
#define VISITOR_HPP

#include <memory>
#include <string>
#include "architecture.hpp"

template <class T>
class Visitor {
   public:
    Visitor(const Architecture& architecture);

    T visit();

   protected:
    virtual void startUp(const Architecture& architecture){};
    virtual void beginNodes(){};
    virtual void onNode(std::shared_ptr<const Node>) = 0;
    virtual void endNodes(){};
    virtual void beginConnections(){};
    virtual void onConnection(std::shared_ptr<const Connection>) = 0;
    virtual void endConnections(){};
    virtual void tearDown(){};

    T _content;

   private:
    const Architecture& _architecture;
};

template class Visitor<std::string>;

#endif  // VISITOR_HPP
