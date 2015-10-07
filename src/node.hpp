/* See LICENSE file for copyright and license details. */

#ifndef __NODE_HPP__B106B138_3E21_47E2_A975_3E5CC0EEB0BF
#define __NODE_HPP__B106B138_3E21_47E2_A975_3E5CC0EEB0BF

#include <string>
#include <memory>
#include <set>
#include <tuple>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>


enum class PortDirection {OUT, IN};

enum class PortType {EXPLICIT, LATENT, OTHER};

template <typename S, typename T>
class node_socket {

};


/**
 * representation of a node. this may be used to form a graphical representation
 */
struct Node
{
public:
    // Port: (name, is_input)
    typedef std::tuple<std::string, PortDirection, PortType> Port;


    Node();

    boost::uuids::uuid uuid;
    bool to_be_deleted; // used to mark node for deletion in the architecture

    std::string name;
    std::set<Port> ports;

    void addPort(const Port port) {ports.insert(port);}

};

typedef std::shared_ptr<Node> NodePtr;
typedef std::shared_ptr<const Node> ConstNodePtr;


#endif /* __NODE_HPP__B106B138_3E21_47E2_A975_3E5CC0EEB0BF */

