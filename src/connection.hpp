/* See LICENSE file for copyright and license details. */

#ifndef __EDGE_HPP__15CD9D15_3771_45FB_A983_227B5DA2A843
#define __EDGE_HPP__15CD9D15_3771_45FB_A983_227B5DA2A843

#include<string>
#include<memory>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "node.hpp"

struct Socket
{
    NodeWeakPtr node;
    PortWeakPtr port;
};

/**
 * representation of a connection  between two nodes.
 */
struct Connection
{
public:


    Connection() : uuid(boost::uuids::random_generator()()) {};


    boost::uuids::uuid uuid;

    std::string name;
    Socket from, to;
    std::string desc;
};
typedef std::shared_ptr<Connection> ConnectionPtr;
typedef std::weak_ptr<Connection> ConnectionWeakPtr;
typedef std::shared_ptr<const Connection> ConstConnectionPtr;

#endif /* __EDGE_HPP__15CD9D15_3771_45FB_A983_227B5DA2A843 */

