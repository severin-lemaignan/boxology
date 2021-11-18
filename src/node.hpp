#ifndef __NODE_HPP
#define __NODE_HPP

#include <QObject>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <tuple>

#include "label.hpp"

class Architecture;

struct Port {
   public:
    enum class Direction { OUT, IN };
    enum class Type { EXPLICIT, LATENT, EVENT, OTHER };

    static const std::map<Type, std::string> TYPENAME;

    Port() {}
    Port(std::string name, Direction direction, Type type)
        : name(name), direction(direction), type(type) {}

    friend bool operator<(const Port& l, const Port& r) {
        return l.name < r.name;
    }

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
    Node(boost::uuids::uuid uuid);
    ~Node();

    NodePtr duplicate() const;

    std::string name() const { return _name; }
    void name(const std::string& name);

    Label label() const { return _label; }
    void label(Label label);

    // geometry -- 100% optional -- may not be set -- only used for
    // visualization needs
    double x() const { return _x; }
    void x(double x) { _x = x; }
    double y() const { return _y; }
    void y(double y) { _y = y; }
    double width() const { return _width; }
    void width(double width) { _width = width; }
    double height() const { return _height; }
    void height(double height) { _height = height; }

    PortPtr createPort(const Port port);
    void remove_port(PortPtr port);
    PortPtr port(const std::string& name);

    const std::set<PortPtr> ports() const { return _ports; }

    boost::uuids::uuid uuid;

    std::shared_ptr<Architecture> sub_architecture;

   signals:
    void dirty();

   private:
    // the node's geometry in whatever 2D space
    double _x, _y, _width, _height;

    std::string _name;
    Label _label;
    std::set<PortPtr> _ports;
};

#endif  // __NODE_HPP
