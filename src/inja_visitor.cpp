#include "visitor.hpp"
#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#include "inja_visitor.hpp"

#include <curl/curl.h>

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

const double pix2mm = 0.13;

vector<string> split(string s, const char separator);
bool contains(const nlohmann::json &container, const nlohmann::json &value);
tuple<string, string> prepareTopic(string topic_in);

static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

InjaVisitor::InjaVisitor(const Architecture &architecture,
                         const string &input_tpl, const string &output_path)
    : Visitor(architecture), input_tpl(input_tpl), output_path(output_path) {
  fs::path tpl_path;

  if (!fs::exists(input_tpl)) {

    cerr << "[EE] Template " << input_tpl << " not found!" << endl;
    return;
  }

  cerr << "Using template " << input_tpl << endl;
  env_ = make_unique<inja::Environment>(fs::path(input_tpl).parent_path(),
                                        fs::path(output_path).parent_path());

  env_->set_expression("<<", ">>");
  env_->set_line_statement("$$$$$");
  env_->set_comment("{##", "##}"); // Comments
  env_->set_trim_blocks(true);
  env_->set_lstrip_blocks(true);

  env_->add_callback("to_mm", 1, [](inja::Arguments &args) {
    float number = args.at(0)->get<float>();
    return number * pix2mm;
  });

  env_->add_callback("make_id", 1, [this](inja::Arguments &args) {
    auto raw = args.at(0)->get<string>();
    auto [id, _] = this->make_id(raw);
    return id;
  });

  env_->add_callback("substr", 3, [](inja::Arguments &args) {
    auto raw = args.at(0)->get<string>();
    return raw.substr(args.at(1)->get<int>(), args.at(2)->get<int>());
  });

  env_->add_callback("tex_escape", 1, [this](inja::Arguments &args) {
    auto raw = args.at(0)->get<string>();
    return this->tex_escape(raw);
  });
}

void InjaVisitor::startUp() {
  data_["path"] = fs::path(output_path).parent_path().string();
  data_["name"] = architecture.name;
  auto [id, id_capitalized] = make_id(architecture.name);
  data_["id"] = id;
  data_["boxology_version"] = STR(BOXOLOGY_VERSION);
  data_["version"] = architecture.version;
  data_["description"] = architecture.description;
  data_["labels"] = nlohmann::json::array();
  for (const auto &kv : LABEL_NAMES) {
    auto [id, id_capitalized] = make_id(kv.second);
    auto color = LABEL_COLORS.at(kv.first);
    data_["labels"].push_back(nlohmann::json::object({
        {"id", id},
        {"name", kv.second},
        {"color", color},
    }));
  }
}

void InjaVisitor::tearDown() {
  if (!env_) {
    return;
  }

  std::sort(data_["nodes"].begin(), data_["nodes"].end(),
            [](const nlohmann::json &n1, const nlohmann::json &n2) -> bool {
              return n1["id"] < n2["id"];
            });
  std::sort(data_["edges"].begin(), data_["edges"].end(),
            [](const nlohmann::json &e1, const nlohmann::json &e2) -> bool {
              return e1["id"] < e2["id"];
            });

  auto [id, id_capitalized] = make_id(architecture.name);

  cerr << "Generating " << output_path << " using " << input_tpl << "..."
       << endl;
  auto tpl = env_->parse_template(input_tpl);
  // env_->write(tpl, data_, output_path);
  auto out = env_->render(tpl, data_);
  cout << out << endl;

  cerr << "Generation complete: " << output_path << endl;
}

void InjaVisitor::beginNodes() {}

void InjaVisitor::onNode(shared_ptr<const Node> node) {

  nlohmann::json jnode;

  auto name = node->name().substr(0, node->name().find("[") - 1);
  if (name.find("DEPENDENCY:") != string::npos) {
    name = name.substr(string("DEPENDENCY:").size());
  }

  jnode["bin"] = "node"; // default node name in the MD template is 'node'

  jnode["x"] = node->x();
  jnode["y"] = node->y();
  jnode["width"] = node->width();
  jnode["height"] = node->height();

  jnode["description"] = "";
  jnode["short_description"] = "";
  jnode["repo"] = "";

  if (name.find("MOCK: ") == string::npos) {
    // node should *not* be mocked-up

    if (!node->sub_architecture ||
        node->sub_architecture->description.size() == 0) {
      jnode["generate"] = true;
    } else {
      jnode["generate"] = false;

      stringstream ss(node->sub_architecture->description);
      string line;
      while (std::getline(ss, line, '\n')) {
        if (line.find("BRIEF:") != string::npos) {
          jnode["short_description"] = line.substr(string("BRIEF:").size());
          continue;
        }
        if (line.find("FETCH_DOC:") != string::npos) {
          string url = line.substr(string("FETCH_DOC:").size());
          CURLcode res;
          std::string readBuffer;
          CURL *curl = curl_easy_init();
          curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
          curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
          res = curl_easy_perform(curl);
          jnode["description"] = readBuffer;
          continue;
        }
        if (line.find("REPO:") != string::npos) {
          jnode["repo"] = line.substr(string("REPO:").size());
          continue;
        }
        if (line.find("SUBFOLDER:") != string::npos) {
          jnode["repo_subfolder"] = line.substr(string("SUBFOLDER:").size());
          continue;
        }
        if (line.find("BIN:") != string::npos) {
          jnode["bin"] = line.substr(string("BIN:").size());
          continue;
        }
        if (line.find("NOT EXECUTABLE") != string::npos) {
          jnode["bin"] = "";
          continue;
        }
        jnode["description"] = jnode["description"].get<string>() + "\n" + line;
      }
    }
  } else {
    jnode["generate"] = true;
    name = name.substr(node->name().find("MOCK: ") + 6);
  }

  auto [id, id_capitalized] = make_id(name);
  jnode["id"] = id;
  jnode["id_capitalized"] = id_capitalized;
  jnode["name"] = name;
  jnode["type"] = NODE_TYPE_NAMES.at(get_node_type(name));

  auto [label_id, _] = make_id(LABEL_NAMES.at(node->label()));
  jnode["label"] = label_id;

  jnode["dependencies"] = nlohmann::json::array();

  jnode["boxology_version"] = STR(BOXOLOGY_VERSION);

  if (node->sub_architecture) {
    jnode["version"] = node->sub_architecture->version;
  } else {
    jnode["version"] = "1.0.0";
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

      cerr << "[II] Node " << jnode["id"] << ": "
           << (isInput ? "subscribes to" : "publishes") << " topic "
           << jport["topic"] << " (short: " << jport["short"] << ") of type "
           << jport["type"] << endl;

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

      cerr << "[II] Node " << jnode["id"] << ": "
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
  for (const auto &dep : jnode["dependencies"]) {
    packages.insert(string(dep[0]));
  }
  for (const auto &p : packages) {
    jnode["packages"].push_back(p);
  }

  data_["nodes"].push_back(jnode);

  nodes_.push_back(node);
}

void InjaVisitor::endNodes() {}

void InjaVisitor::beginConnections() {}

void InjaVisitor::onConnection(shared_ptr<const Connection> connection) {

  auto from = connection->from.node.lock();
  auto to = connection->to.node.lock();

  auto [from_id, from_id_capitalized] = make_id(from->name());
  auto [to_id, to_id_capitalized] = make_id(to->name());

  auto name = connection->name;
  trim(name);
  if (name == "anonymous") {
    name = "";
  }

  auto [name_id, _] = make_id(name);

  auto id = from_id + "_" + to_id + "_" + name_id;

  auto edge_type = get_edge_type(name);

  auto x_to = to->x() + to->width() / 2;
  auto y_to = to->y() + to->height() / 2;
  auto x_from = from->x() + from->width() / 2;
  auto y_from = from->y() + from->height() / 2;

  int out = 180 / M_PI * atan2(-y_to + y_from, x_to - x_from);
  int in = out + 180;

  nlohmann::json jnode;

  jnode["id"] = id;
  jnode["name"] = name;
  jnode["from"] = from_id;
  jnode["to"] = to_id;
  jnode["out_angle"] = out;
  jnode["in_angle"] = in;
  jnode["type"] = ROS_TYPE_NAMES.at(edge_type);

  data_["edges"].push_back(jnode);
}
