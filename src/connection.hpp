/* See LICENSE file for copyright and license details. */

#ifndef __EDGE_HPP__15CD9D15_3771_45FB_A983_227B5DA2A843
#define __EDGE_HPP__15CD9D15_3771_45FB_A983_227B5DA2A843

#include<string>
#include<memory>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "node.hpp"

enum class ConnectionType {EVENT, OTHER};

/**
 * representation of a connection  between two nodes.
 */
struct Connection
{
public:

    typedef std::pair<ConstNodePtr, size_t> Port;

    Connection() :
        uuid(boost::uuids::random_generator()()),
        to_be_deleted(false) {};


    boost::uuids::uuid uuid;
    bool to_be_deleted; // used to mark connection for deletion in the architecture

    std::string name;
    Port from, to;
    ConnectionType type;
    std::string desc;
};
typedef std::shared_ptr<Connection> ConnectionPtr;
typedef std::shared_ptr<const Connection> ConstConnectionPtr;

#endif /* __EDGE_HPP__15CD9D15_3771_45FB_A983_227B5DA2A843 */

