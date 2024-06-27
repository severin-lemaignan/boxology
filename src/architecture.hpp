#ifndef ARCHITECTURE_HPP
#define ARCHITECTURE_HPP

#define DEBUG(x)                                                               \
  do {                                                                         \
    std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] " << x;              \
  } while (0)

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <memory>
#include <set>
#include <string>
#include <utility> // for std::pair

#include "connection.hpp"
#include "node.hpp"

namespace Json {
class Value;
}

class Architecture {
public:
  typedef std::set<NodePtr> Nodes;
  typedef std::set<ConnectionPtr> Connections;
  typedef std::pair<std::set<NodePtr>, std::set<ConnectionPtr>>
      NodesAndConnections;
  typedef std::pair<NodesAndConnections, NodesAndConnections> ToAddToRemove;

  Architecture();
  Architecture(boost::uuids::uuid uuid);

  bool operator=(const Architecture &arch) const { return uuid == arch.uuid; }
  bool operator<(const Architecture &arch) const { return uuid < arch.uuid; }

  ToAddToRemove load(const std::string &filename);

  NodePtr createNode(bool silent = false);
  NodePtr createNode(const boost::uuids::uuid &uuid, bool silent = false);

  void addNode(NodePtr node, bool silent = false);
  void removeNode(NodePtr node);
  NodePtr node(const boost::uuids::uuid &uuid);

  Nodes nodes() { return _nodes; }
  const Nodes nodes() const { return _nodes; }

  ConnectionPtr createConnection(Socket from, Socket to);
  ConnectionPtr createConnection(const boost::uuids::uuid &uuid, Socket from,
                                 Socket to);

  void removeConnection(Socket from, Socket to);

  Connections connections() { return _connections; }
  const Connections connections() const { return _connections; }

  boost::uuids::uuid uuid;

  std::string name;
  std::string version;
  std::string description;
  std::string filename;

private:
  ToAddToRemove load(const Json::Value &json, const boost::uuids::uuid uuid,
                     bool clearFirst = true, bool recreateUUIDs = false,
                     bool metadata = true, bool silent = false);

  Nodes _nodes;
  Connections _connections;

  boost::uuids::uuid get_uuid(const std::string &uuid,
                              const std::string &ctxt = "");
};

#endif // ARCHITECTURE_HPP
