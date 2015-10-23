#define DEBUG(x)                                                      \
    do {                                                              \
        std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] " << x; \
    } while (0)

#include <iostream>

#include <QStandardPaths>
#include <QDirIterator>

#include "json/json.h"

#include "architecture_manager.hpp"


using namespace std;

ArchitectureManager::ArchitectureManager() {
    
    initialize();
}

void ArchitectureManager::initialize() {

    // look for all models in the current path
    initializeFromPath(QDir());

    // look for all models in the application shared data path
    auto dir = QDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));

    if(!dir.cd(QString::fromStdString(CONFIG_PATH))) {
        dir.mkdir(QString::fromStdString(CONFIG_PATH));
        dir.cd(QString::fromStdString(CONFIG_PATH));
        cout << "Created " << dir.path().toStdString() << endl;
    }

    initializeFromPath(dir, true);

}

void ArchitectureManager::initializeFromPath(const QDir& path, bool recursive) {

    QDir dir(path);

    QStringList filters;
    filters << "*.json";
    dir.setNameFilters(filters);

    QDirIterator it(dir, 
                    recursive ? QDirIterator::Subdirectories | QDirIterator::FollowSymlinks : QDirIterator::FollowSymlinks);

    while(it.hasNext()) {
        load_model(it.next().toStdString());
    }

    cout << _models.size() << " models loaded." << endl;

}

void ArchitectureManager::list_models() {

    for(const auto& kv : _models) {
        cout << "- " << kv.second->name << " (v." << kv.second->version << ")" << endl;
        cout << "  (in " << kv.second->filename << ")" << endl;
    }
}

Architecture* ArchitectureManager::get_model(const boost::uuids::uuid& uuid) const {
    if(_models.count(uuid) > 0) {
        return _models.at(uuid).get();
    }
    return nullptr;
}

void ArchitectureManager::load_model(const string& path) {

    auto arch = new Architecture();

    try {
        arch->load(path);
    } catch (Json::RuntimeError jre) {
        DEBUG("Unable to load " << path << ": JSON syntax error.");
        return;
    } catch (runtime_error e) {
        DEBUG("Unable to load " << path << ": " << e.what());
        return;
    }

    if(_models.count(arch->uuid) > 0) {
        DEBUG("Architecture " << arch->name << " is duplicated:" << endl);
        DEBUG("\t- " << _models[arch->uuid]->filename << endl);
        DEBUG("\t- " << arch->filename << endl);
        DEBUG("Skipping the second one." << endl);
        return;
    }

    _models[arch->uuid] = unique_ptr<Architecture>(arch);

}
