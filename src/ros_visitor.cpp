#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#include "ros_visitor.hpp"

#include <QDir>
#include <QStandardPaths>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>

#include "inja/inja.hpp"
#include "node.hpp"

using namespace std;
namespace fs = std::filesystem;

RosVisitor::RosVisitor(const Architecture& architecture, const string& ws_path)
    : Visitor(architecture), ws_path(ws_path) {
    string tpl_path;

    for (auto p :
         QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)) {
        auto path = QDir(p + "/templates/ros");
        cout << "Looking for ROS templates in "
             << path.absolutePath().toStdString() << endl;

        if (path.exists()) {
            tpl_path = (path.absolutePath() + QDir::separator()).toStdString();
            break;
        }
    }

    if (!tpl_path.empty()) {
        cout << "Using ROS templates found at " << tpl_path << endl;
        env_ = make_unique<inja::Environment>(
            tpl_path, (QDir(QString::fromStdString(ws_path)).absolutePath() +
                       QDir::separator())
                          .toStdString());
        env_->set_line_statement("$$$$$");
    } else {
        cout << "[EE] ROS templates not found! Can not generate ROS nodes."
             << endl;
    }
}

void RosVisitor::startUp() {
    data_["path"] = ws_path;
    data_["name"] = architecture.name;
    data_["boxology_version"] = STR(BOXOLOGY_VERSION);
    data_["version"] = architecture.version;
    data_["description"] = architecture.description;
}

void RosVisitor::tearDown() {
    if (!env_) return;

    static const vector<string> tpls{"package.xml", "CMakeLists.txt",
                                     "src/main.cpp"};

    for (auto node : nodes_) {
        cout << "Generating " << node->name() << "..." << endl;

        auto id = make_id(node->name());

        auto rel_path = fs::path("src") / id;
        auto abs_path = fs::path(ws_path) / rel_path;
        auto src_path = abs_path / "src";
        fs::create_directories(src_path);  // will also create 'path'

        data_["project_name"] = id;

        for (const auto& file : tpls) {
            auto tpl = env_->parse_template(file);
            // cout << "\t- " << (abs_path / file).string() << endl;
            env_->write(tpl, data_, (rel_path / file).string());
        }
    }

    cout << "Generation of ROS nodes complete. The generated nodes can be "
            "found in "
         << ws_path << endl;
}

void RosVisitor::beginNodes() {}

void RosVisitor::onNode(shared_ptr<const Node> node) { nodes_.push_back(node); }

void RosVisitor::endNodes() {}

void RosVisitor::beginConnections() {}

void RosVisitor::onConnection(shared_ptr<const Connection> connection) {}

