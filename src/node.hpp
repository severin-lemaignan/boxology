/* See LICENSE file for copyright and license details. */

#ifndef __NODE_HPP__B106B138_3E21_47E2_A975_3E5CC0EEB0BF
#define __NODE_HPP__B106B138_3E21_47E2_A975_3E5CC0EEB0BF

#include <string>
#include <memory>
#include <set>
#include <tuple>

#include <QObject>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

struct Port {
   public:
    enum class Direction { OUT, IN };
    enum class Type { EXPLICIT, LATENT, OTHER };

    Port() : uuid(boost::uuids::random_generator()()) {}
    Port(std::string name, Direction direction, Type type)
        : uuid(boost::uuids::random_generator()()),
          name(name),
          direction(direction),
          type(type) {}

    friend bool operator<(const Port& l, const Port& r) {
        return l.uuid < r.uuid;
    }

    boost::uuids::uuid uuid;

    std::string name;
    Direction direction;
    Type type;
};

typedef std::shared_ptr<Port> PortPtr;
typedef std::weak_ptr<Port> PortWeakPtr;
typedef std::shared_ptr<const Port> ConstPortPtr;

/**
 * representation of a node. this may be used to form a graphical representation
 */
struct Node;
typedef std::shared_ptr<Node> NodePtr;
typedef std::weak_ptr<Node> NodeWeakPtr;
typedef std::shared_ptr<const Node> ConstNodePtr;

struct Node : public QObject {
    Q_OBJECT
   public:
    Node();
    ~Node();

    boost::uuids::uuid uuid;

    NodePtr duplicate() const;

    std::string name() const { return _name; }
    void name(const std::string& name);

    const std::set<PortPtr> ports() const { return _ports; }
    PortPtr createPort(const Port port);

signals:
    void dirty();

   private:
    std::string _name;
    std::set<PortPtr> _ports;
};

#endif /* __NODE_HPP__B106B138_3E21_47E2_A975_3E5CC0EEB0BF */
