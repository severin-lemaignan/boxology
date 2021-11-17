#include <nlohmann/json_fwd.hpp>
#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#include <QDir>
#include <QStandardPaths>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>

#include "cognitive_function.hpp"
#include "inja/inja.hpp"
#include "node.hpp"
#include "ros_visitor.hpp"

using namespace std;
namespace fs = std::filesystem;

RosVisitor::RosVisitor(const Architecture& architecture, const string& ws_path)
    : Visitor(architecture), ws_path(ws_path) {
    fs::path tpl_path;

    for (auto p :
         QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)) {
        tpl_path = fs::path(p.toStdString()) / "templates" / "ros";
        cout << "Looking for ROS templates in " << tpl_path << endl;

        if (fs::exists(tpl_path)) {
            break;
        }
    }

    if (!tpl_path.empty()) {
        cout << "Using ROS templates found at " << tpl_path << endl;
        env_ = make_unique<inja::Environment>(
            (tpl_path / "default").string() + fs::path::preferred_separator,
            ws_path + fs::path::preferred_separator);

        env_->set_line_statement("$$$$$");
        env_->set_trim_blocks(true);
        env_->set_lstrip_blocks(true);

        env_main_node_ = make_unique<inja::Environment>(
            (tpl_path / "main_node").string() + fs::path::preferred_separator,
            ws_path + fs::path::preferred_separator);

        env_main_node_->set_line_statement("$$$$$");
        env_main_node_->set_trim_blocks(true);
        env_main_node_->set_lstrip_blocks(true);
    } else {
        cout << "[EE] ROS templates not found! Can not generate ROS nodes."
             << endl;
    }
}

void RosVisitor::startUp() {
    data_["path"] = ws_path;
    data_["name"] = architecture.name;
    data_["id"] = make_id(architecture.name);
    data_["boxology_version"] = STR(BOXOLOGY_VERSION);
    data_["version"] = architecture.version;
    data_["description"] = architecture.description;
}

void RosVisitor::tearDown() {
    if (!env_) return;

    ///////////////////////////////////////////////////////
    // Create parent node with main launchfile
    //
    vector<string> main_node_tpls{"package.xml", "CMakeLists.txt",
                                  "launch/start_all.launch"};

    auto id = make_id(architecture.name);
    auto rel_path = fs::path("src") / id;
    auto abs_path = fs::path(ws_path) / rel_path;
    auto launch_path = abs_path / "launch";
    fs::create_directories(launch_path);  // will also create 'path'

    for (const auto& file : main_node_tpls) {
        auto tpl = env_main_node_->parse_template(file);
        env_main_node_->write(tpl, data_, (rel_path / file).string());
    }

    ///////////////////////////////////////////////////////
    // Create all the nodes
    //
    vector<string> default_tpls{"package.xml", "CMakeLists.txt",
                                "src/main.cpp"};

    for (auto node : data_["nodes"]) {
        string id(node["id"]);
        cout << "Generating " << node["name"] << " as node [" << id << "]..."
             << endl;

        auto rel_path = fs::path("src") / id;
        auto abs_path = fs::path(ws_path) / rel_path;
        auto src_path = abs_path / "src";
        fs::create_directories(src_path);  // will also create 'path'

        for (const auto& file : default_tpls) {
            auto tpl = env_->parse_template(file);
            // cout << "\t- " << (abs_path / file).string() << endl;
            env_->write(tpl, node, (rel_path / file).string());
        }
    }

    cout << "Generation of ROS nodes complete. The generated nodes can be "
            "found in "
         << ws_path << endl;
}

void RosVisitor::beginNodes() {}

void RosVisitor::onNode(shared_ptr<const Node> node) {
    // ignore TF nodes
    if (node->name() == "TF" || node->name() == "tf") return;

    nlohmann::json jnode;

    auto name = node->name().substr(0, node->name().find("["));

    jnode["id"] = make_id(name);
    jnode["name"] = name;
    jnode["label"] = COGNITIVE_FUNCTION_NAMES.at(node->cognitive_function());

    jnode["boxology_version"] = STR(BOXOLOGY_VERSION);

    if (node->sub_architecture) {
        jnode["version"] = node->sub_architecture->version;
        jnode["description"] = node->sub_architecture->description;
    } else {
        jnode["version"] = "1.0.0";
        jnode["description"] = "";
    }

    data_["nodes"].push_back(jnode);

    nodes_.push_back(node);
}

void RosVisitor::endNodes() {}

void RosVisitor::beginConnections() {}

void RosVisitor::onConnection(shared_ptr<const Connection> connection) {}

