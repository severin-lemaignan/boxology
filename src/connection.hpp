/* See LICENSE file for copyright and license details. */

#ifndef __EDGE_HPP__15CD9D15_3771_45FB_A983_227B5DA2A843
#define __EDGE_HPP__15CD9D15_3771_45FB_A983_227B5DA2A843

#include <string>
#include <memory>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "node.hpp"

struct Socket {
    NodeWeakPtr node;
    PortWeakPtr port;
};

inline bool operator==(const Socket& l, const Socket& r) {
    if (l.node.expired() || r.node.expired()) return false;
    if (l.node.lock() == r.node.lock()) return l.port.lock() == r.port.lock();
    return l.node.lock() == r.node.lock();
}

/**
 * representation of a connection  between two nodes.
 */
struct Connection {
   public:
    static const std::string ANONYMOUS;

    Connection() : uuid(boost::uuids::random_generator()()), name(ANONYMOUS){};

    Connection(const boost::uuids::uuid& uuid) : uuid(uuid), name(ANONYMOUS){};

    // TODO: attention: possible discrepency between UUIDs and equality
    // operator!! Non intuitive!!
    inline bool operator==(const Connection& r) {
        return (to == r.to) && (from == r.from);
    }

    ~Connection();

    boost::uuids::uuid uuid;

    std::string name;
    Socket from, to;
    std::string desc;
};
typedef std::shared_ptr<Connection> ConnectionPtr;
typedef std::weak_ptr<Connection> ConnectionWeakPtr;
typedef std::shared_ptr<const Connection> ConstConnectionPtr;

#endif /* __EDGE_HPP__15CD9D15_3771_45FB_A983_227B5DA2A843 */
