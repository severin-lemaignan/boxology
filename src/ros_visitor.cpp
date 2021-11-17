#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#include "ros_visitor.hpp"

#include <QStandardPaths>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <regex>
#include <string>

#include "cognitive_function.hpp"
#include "inja/inja.hpp"
#include "node.hpp"

using namespace std;
namespace fs = std::filesystem;

vector<string> split(string s, const char separator) {
    istringstream ss;
    ss.str(s);
    string segment;
    std::vector<std::string> seglist;
    while (std::getline(ss, segment, separator)) {
        seglist.push_back(segment);
    }
    return seglist;
}

bool contains(const nlohmann::json& container, const nlohmann::json& value) {
    for (const auto& i : container) {
        if (i[0] == value[0] && i[1] == value[1]) {
            return true;
        }
    }
    return false;
}

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

    jnode["dependencies"] = nlohmann::json::array();

    jnode["boxology_version"] = STR(BOXOLOGY_VERSION);

    if (node->sub_architecture) {
        jnode["version"] = node->sub_architecture->version;
        jnode["description"] = node->sub_architecture->description;
    } else {
        jnode["version"] = "1.0.0";
        jnode["description"] = "";
    }

    //////////////////////////////////////////////
    // Ports
    //
    jnode["inputs"] = nlohmann::json::array();
    jnode["outputs"] = nlohmann::json::array();
    for (auto p : node->ports()) {
        nlohmann::json jport;

        bool isInput = (p->direction == Port::Direction::IN);

        auto name = p->name;

        regex topic_regex("`(.*)` \\[(.*)\\]", regex_constants::ECMAScript);
        smatch topic_matches;
        regex tf_regex("`tf: (.*)`", regex_constants::ECMAScript);
        smatch tf_matches;

        if (regex_search(name, topic_matches, topic_regex)) {
            jport["type"] = "topic";
            jport["topic"] = topic_matches[1];
            jport["short"] = split(jport["topic"], '/').back();
            auto type = split(topic_matches[2], '/');
            jport["datatype"] = {type.front(), type.back()};
            if (!contains(jnode["dependencies"],jport["datatype"])) {
                jnode["dependencies"].push_back(jport["datatype"]);
            }

            cout << "[II] Node " << jnode["id"] << ": "
                 << (isInput ? "subscribes to" : "publishes") << " topic "
                 << jport["topic"] << " (short: " << jport["shorttopic"]
                 << ") of type " << jport["type"] << endl;

        } else if (regex_search(name, tf_matches, tf_regex)) {
            jport["type"] = "tf";
            jport["frame"] = tf_matches[1];

            if (isInput) {
                jport["datatype"] = {"tf", "transform_listener"};
            }else {
                jport["datatype"] = {"tf", "transform_broadcaster"};
            }
            if (!contains(jnode["dependencies"],jport["datatype"])) {
                jnode["dependencies"].push_back(jport["datatype"]);
            }

            cout << "[II] Node " << jnode["id"] << ": "
                 << (isInput ? "listen to" : "broadcasts") << " TF frame "
                 << jport["frame"] << endl;
        } else {
            jport["type"] = "undefined";
            jport["description"] = name;
            jport["topic"] = make_id(name);
            jport["short"] = make_id(name);
            jport["datatype"] = {"std_msgs", "Empty"};
            if (!contains(jnode["dependencies"], jport["datatype"])) {
                jnode["dependencies"].push_back(jport["datatype"]);
            }


        }

        if (isInput) {
            jnode["inputs"].push_back(jport);
        } else {
            jnode["outputs"].push_back(jport);
        }
    }

    data_["nodes"].push_back(jnode);

    nodes_.push_back(node);
}

void RosVisitor::endNodes() {}

void RosVisitor::beginConnections() {}

void RosVisitor::onConnection(shared_ptr<const Connection> connection) {}

