#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#include "md_visitor.hpp"

#include <QStandardPaths>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <regex>
#include <string>

#include "inja/inja.hpp"
#include "label.hpp"
#include "node.hpp"

using namespace std;
namespace fs = std::filesystem;

vector<string> split(string s, const char separator);
bool contains(const nlohmann::json& container, const nlohmann::json& value);
tuple<string, string> prepareTopic(string topic_in);

MdVisitor::MdVisitor(const Architecture& architecture, const string& ws_path)
    : Visitor(architecture), ws_path(ws_path) {
    fs::path tpl_path;

    for (auto p :
         QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)) {
        tpl_path = fs::path(p.toStdString()) / "templates" / "md";
        cout << "Looking for Markdown templates in " << tpl_path << endl;

        if (fs::exists(tpl_path)) {
            break;
        }
        tpl_path = "";
    }

    if (!tpl_path.empty()) {
        cout << "Using Markdown templates found at " << tpl_path << endl;
        env_ = make_unique<inja::Environment>(
            tpl_path.string() + fs::path::preferred_separator,
            ws_path + fs::path::preferred_separator);

        env_->set_line_statement("$$$$$");
        env_->set_comment("{##", "##}");  // Comments
        env_->set_trim_blocks(true);
        env_->set_lstrip_blocks(true);

    } else {
        cout << "[EE] Markdown templates not found! Can not generate Markdown "
                "output."
             << endl;
    }
}

void MdVisitor::startUp() {
    data_["path"] = ws_path;
    data_["name"] = architecture.name;
    auto [id, id_capitalized] = make_id(architecture.name);
    data_["id"] = id;
    data_["boxology_version"] = STR(BOXOLOGY_VERSION);
    data_["version"] = architecture.version;
    data_["description"] = architecture.description;
}

void MdVisitor::tearDown() {
    if (!env_) return;

    vector<string> tpls{"architecture.md"};

    auto [id, id_capitalized] = make_id(architecture.name);

    for (const auto& file : tpls) {
        cout << "Generating " << file << "..." << endl;
        auto tpl = env_->parse_template(file);
        env_->write(tpl, data_, file);
    }

    cout << "Generation of Markdown complete. The generated files can be "
            "found in "
         << ws_path << endl;
}

void MdVisitor::beginNodes() {}

void MdVisitor::onNode(shared_ptr<const Node> node) {
    // ignore TF nodes
    if (node->name() == "TF" || node->name() == "tf") return;

    nlohmann::json jnode;

    auto name = node->name().substr(0, node->name().find("[") - 1);
    if (name.find("DEPENDENCY:") != string::npos) {
        name = name.substr(string("DEPENDENCY:").size());
    }

    auto [id, id_capitalized] = make_id(name);
    jnode["id"] = id;
    jnode["id_capitalized"] = id_capitalized;
    jnode["name"] = name;
    jnode["label"] = LABEL_NAMES.at(node->label());

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
        jport["name"] = name;

        regex topic_regex("^(/.*) \\[(.*)\\]$", regex_constants::ECMAScript);
        smatch topic_matches;
        regex tf_regex("^tf: (.*)$", regex_constants::ECMAScript);
        smatch tf_matches;

        if (regex_search(name, topic_matches, topic_regex)) {
            jport["type"] = "topic";
            auto [topic, shortname] = prepareTopic(topic_matches[1]);
            jport["topic"] = topic;
            jport["short"] = shortname;
            auto type = split(topic_matches[2], '/');
            jport["datatype"] = {type.front(), type.back()};
            if (!contains(jnode["dependencies"], jport["datatype"])) {
                jnode["dependencies"].push_back(jport["datatype"]);
            }

            cout << "[II] Node " << jnode["id"] << ": "
                 << (isInput ? "subscribes to" : "publishes") << " topic "
                 << jport["topic"] << " (short: " << jport["short"]
                 << ") of type " << jport["type"] << endl;

        } else if (regex_search(name, tf_matches, tf_regex)) {
            jport["type"] = "tf";
            jport["frame"] = tf_matches[1];

            if (isInput) {
                jport["datatype"] = {"tf", "transform_listener"};
            } else {
                jport["datatype"] = {"tf", "transform_broadcaster"};
            }
            if (!contains(jnode["dependencies"], jport["datatype"])) {
                jnode["dependencies"].push_back(jport["datatype"]);
            }

            cout << "[II] Node " << jnode["id"] << ": "
                 << (isInput ? "listen to" : "broadcasts") << " TF frame "
                 << jport["frame"] << endl;
        } else {
            jport["type"] = "undefined";
            auto [id, id_capitalized] = make_id(name);
            jport["topic"] = id;
            jport["short"] = id;
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

    // ensure package dependencies are only listed *one* time
    // otherwise catkin complains.
    set<string> packages;
    for (const auto& dep : jnode["dependencies"]) {
        packages.insert(string(dep[0]));
    }
    for (const auto& p : packages) {
        jnode["packages"].push_back(p);
    }

    data_["nodes"].push_back(jnode);

    nodes_.push_back(node);
}

void MdVisitor::endNodes() {}

void MdVisitor::beginConnections() {}

void MdVisitor::onConnection(shared_ptr<const Connection> connection) {}

