/* See LICENSE file for copyright and license details. */

#ifndef __NODE_HPP__B106B138_3E21_47E2_A975_3E5CC0EEB0BF
#define __NODE_HPP__B106B138_3E21_47E2_A975_3E5CC0EEB0BF

#include<string>
#include<memory>
#include<set>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

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
    typedef std::pair<std::string, bool> Port;


    Node():
        uuid(boost::uuids::random_generator()()),
        to_be_deleted(false) {};

    boost::uuids::uuid uuid;
    bool to_be_deleted; // used to mark node for deletion in the architecture

    std::string name;
    std::set<Port> ports;

};

typedef std::shared_ptr<Node> NodePtr;
typedef std::shared_ptr<const Node> ConstNodePtr;


#endif /* __NODE_HPP__B106B138_3E21_47E2_A975_3E5CC0EEB0BF */

