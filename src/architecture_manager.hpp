#ifndef ARCHITECTURE_MANAGER
#define ARCHITECTURE_MANAGER

#include <string>
#include <memory>
#include <set>
#include <boost/uuid/uuid.hpp>

#include <QDir>

#include "architecture.hpp"

const std::string CONFIG_PATH="boxology";

class ArchitectureManager {

public:

    ArchitectureManager();

    void list_models();

    Architecture* get_model(const boost::uuids::uuid& uuid) const;

private:
    void initialize();
    void initializeFromPath(const QDir& dir, bool recursive = false);
    void load_model(const std::string& path);

    std::map<boost::uuids::uuid, std::unique_ptr<Architecture>> _models;

};

#endif // ARCHITECTURE_MANAGER
