/* See LICENSE file for copyright and license details. */

#include "graphicsnode.hpp"
#include "node.hpp"

Node::Node():
        uuid(boost::uuids::random_generator()()),
        to_be_deleted(false)
{
}


