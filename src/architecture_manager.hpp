#ifndef ARCHITECTURE_MANAGER
#define ARCHITECTURE_MANAGER

#include <string>
#include <memory>
#include <set>
#include <boost/uuid/uuid.hpp>

#include "architecture.hpp"

const std::string CONFIG_PATH="boxology";

class ArchitectureManager {

public:

    ArchitectureManager();

    void list_models();


private:
    void initialize();
    void load_model(const std::string& path);

    std::map<boost::uuids::uuid, std::unique_ptr<Architecture>> _models;

};

#endif // ARCHITECTURE_MANAGER
