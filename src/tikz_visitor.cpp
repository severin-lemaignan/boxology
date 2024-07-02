#include "tikz_visitor.hpp"

#include <algorithm>
#include <boost/algorithm/string.hpp> // for search and replace
#include <string>

#include "label.hpp"

using namespace std;

void TikzVisitor::startUp() {
  ss << "\\documentclass[tikz]{standalone}" << endl;
  ss << "%\\documentclass[tikz,dvisvgm]{standalone} % generate a SVG via "
        "dvisvgm. This will keep text as text, and supports links."
     << endl;
  ss << "%\\documentclass[tikz,convert=pdf2svg]{standalone} % generate a SVG "
        "file automatically"
     << endl;
  ss << endl;
  ss << "\\usepackage{hyperref}" << endl;
  ss << "%\\usepackage[hypertex]{hyperref} % use this for links with dvisvgm"
     << endl;
  ss << "\\usetikzlibrary{positioning}" << endl;
  ss << "\\usetikzlibrary{shapes.geometric}" << endl;
  ss << endl;
  ss << "\\usepackage{fontspec}" << endl;
  ss << "\\newcommand\\defaultfont\\sffamily" << endl;
  ss << "%\\newfontfamily\\defaultfont[Scale=1.0]{Inter} % use that for custom "
        "font"
     << endl;
  ss << "\\newcommand\\monospace\\ttfamily" << endl;
  ss << "%\\newfontfamily\\monospace[Scale=1.0]{Ubuntu Mono}" << endl;
  ss << endl;

  // Generate colors based on what is used for Boxology's GUI
  for (const auto &kv : LABEL_COLORS) {
    auto color = kv.second;
    transform(color.begin(), color.end(), color.begin(), ::toupper);

    auto id = make_id(LABEL_NAMES.at(kv.first));
    ss << "\\definecolor{" << id << "}{HTML}{" << color.substr(1) << "}"
       << endl;
  }

  ss << "\n\\begin{document}" << endl;
  ss << endl;

  ss << "\\begin{tikzpicture}[" << endl;
  ss << "             font=\\defaultfont," << endl;
  ss << "             >=latex," << endl;
  ss << "             every edge/.style={draw, line width=3pt,opacity=0.5},"
     << endl;
  ss << "             hw_edge/.style={dashed}, " << endl;
  ss << "             topic_edge/.style={color=blue,text=black}, " << endl;
  ss << "             service_edge/.style={color=orange,text=black}, " << endl;
  ss << "             action_edge/.style={color=purple,text=black}, " << endl;
  ss << "             node/.style={draw, rounded corners, align=center, "
        "inner sep=5pt, fill=black!20},"
     << endl;
  ss << "             label/.style={midway, align=center, "
        "fill=white,opacity=0.8},"
     << endl;
  ss << "             topic/.style={midway, align=center, font=\\monospace, "
        "fill=white,opacity=0.8},"
     << endl;
  ss << "             service/.style={midway, align=center, font=\\monospace, "
        "fill=white,opacity=0.8},"
     << endl;
  ss << "             action/.style={midway, align=center, font=\\monospace, "
        "fill=white,opacity=0.8}]"
     << endl;
  ss << endl;
}

void TikzVisitor::tearDown() {
  ss << endl;
  ss << "\\end{tikzpicture}" << endl;
  ss << "\\end{document}" << endl;

  _content = ss.str();
}

void TikzVisitor::beginNodes() { ss << "        %%% NODES" << endl; }

void TikzVisitor::onNode(shared_ptr<const Node> node) {

  auto label_id = make_id(LABEL_NAMES.at(node->label()));
  auto [id, id_capitalized] = get_id(node->uuid);

  ss << "        \\node "
        "at ("
     << tikz_unit(node->x()) << "," << tikz_unit(-node->y())
     << ") "
        "[node, "
     << (is_node_name(node->name()) ? "font=\\monospace, " : "")
     << "anchor=north west, "
        "text width="
     << tikz_unit(node->width())
     << ", "
        "minimum width="
     << tikz_unit(node->width())
     << ", "
        "minimum height="
     << tikz_unit(node->height())
     << ", "
        "fill="
     << label_id << "!50"
     << (label_id == "modeldata"
             ? ", cylinder, shape border rotate=90, aspect=0.25"
             : "")
     << "]"
     << " (" << id << ") {" << sanitize_tex(node->name()) << "};";
  ss << endl;

  // for (const auto port : node->ports()) {
  //    port->name;
  //    (port->direction == Port::Direction::IN ? "in" : "out");
  //}
}

void TikzVisitor::beginConnections() {
  ss << endl;
  ss << "        %%% CONNECTIONS" << endl;
}

void TikzVisitor::onConnection(shared_ptr<const Connection> connection) {
  // connection->name;

  auto from = connection->from.node.lock();
  auto to = connection->to.node.lock();

  auto [from_id, from_id_capitalized] = get_id(from->uuid);
  auto [to_id, to_id_capitalized] = get_id(to->uuid);

  auto name = connection->name;
  trim(name);

  auto edge_type = get_edge_type(name);

  auto x_to = to->x() + to->width() / 2;
  auto y_to = to->y() + to->height() / 2;
  auto x_from = from->x() + from->width() / 2;
  auto y_from = from->y() + from->height() / 2;

  int out = 180 / M_PI * atan2(-y_to + y_from, x_to - x_from);
  int in = out + 180;

  if (name.size() == 0 || name == "anonymous") {
    ss << "        \\path (" << from_id << ") edge[->, "
       << ((is_hardware(from->name()) || is_hardware(to->name())) ? "hw_edge, "
                                                                  : "")
       << "out=" << out << ", in=" << in
       << ", looseness=0.4] "
          "("
       << to_id << ");" << endl;
  } else {

    string label;
    switch (edge_type) {
    case EdgeType::SERVICE:
      label = "\\href{../services/" + make_href_identifier(name) + ".html}{" +
              sanitize_tex(name) + "}";
      break;
    case EdgeType::ACTION:
      label = "\\href{../actions/" + make_href_identifier(name) + ".html}{" +
              sanitize_tex(name) + "}";
      break;
    case EdgeType::TOPIC:
      label = "\\href{../topics/" + make_href_identifier(name) + ".html}{" +
              sanitize_tex(name) + "}";
      break;
    default:
      label = sanitize_tex(name);
    }

    ss << "        \\path (" << from_id << ") edge[->, "
       << ((is_hardware(from->name()) || is_hardware(to->name()))
               ? "hw_edge, "
               : (edge_type == EdgeType::SERVICE
                      ? "service_edge, "
                      : (edge_type == EdgeType::ACTION
                             ? "action_edge, "
                             : (edge_type == EdgeType::TOPIC ? "topic_edge, "
                                                             : ""))))
       << "out=" << out << ", in=" << in << ", looseness=0.4] node["
       << (edge_type == EdgeType::SERVICE
               ? "service"
               : (edge_type == EdgeType::ACTION
                      ? "action"
                      : (edge_type == EdgeType::TOPIC ? "topic" : "label")))
       << "] {" << (edge_type == EdgeType::SERVICE ? "\\textbf{" : "")
       << (edge_type == EdgeType::ACTION ? "\\textbf{" : "") << label
       << (edge_type == EdgeType::SERVICE ? "}" : "")
       << (edge_type == EdgeType::ACTION ? "}" : "")
       << "}"
          "("
       << to_id << ");" << endl;
  }
}

string TikzVisitor::tikz_unit(const double dim) {
  stringstream ss;
  ss << (int)(dim * pix2mm) << "mm";
  return ss.str();
}

string TikzVisitor::make_href_identifier(const std::string &text) {
  string result(text);

  if (result.substr(0, 8) == "service:") {
    result = result.substr(8);
  } else if (result.substr(0, 7) == "action:") {
    result = result.substr(7);
  } else if (result.substr(0, 6) == "topic:") {
    result = result.substr(6);
  } else if (result.substr(0, 5) == "node:") {
    result = result.substr(5);
  } else if (result.substr(0, 3) == "hw:") {
    result = result.substr(3);
  }
  trim(result);

  if (result.substr(0, 1) == "/") {
    result = result.substr(1);
  }

  vector<pair<string, string>> substitutions{
      {"/*", ""},
      {"/", "-"},
  };

  for (const auto &kv : substitutions) {
    boost::replace_all(result, kv.first, kv.second);
  }

  return result;
}

string TikzVisitor::sanitize_tex(const std::string &text) {
  string result(text);

  if (result.substr(0, 8) == "service:") {
    result = result.substr(8);
  } else if (result.substr(0, 7) == "action:") {
    result = result.substr(7);
  } else if (result.substr(0, 6) == "topic:") {
    result = result.substr(6);
  } else if (result.substr(0, 5) == "node:") {
    result = result.substr(5);
  } else if (result.substr(0, 3) == "hw:") {
    result = result.substr(3);
  }
  trim(result);

  // special-case the backslash
  boost::replace_all(result, "\\", "\\textbackslash");

  map<string, string> tex_substitutions{
      {"{", "\\{"}, {"}", "\\}"},
      {"$", "\\$"}, {"&", "\\&"},
      {"#", "\\#"}, {"^", "\\textasciicircum{}"},
      {"_", "\\_"}, {"~", "\\textasciitilde{}"},
      {"%", "\\%"}};

  for (const auto &kv : tex_substitutions) {
    boost::replace_all(result, kv.first, kv.second);
  }

  // finish the backslash special-case
  boost::replace_all(result, "textbackslash", "textbackslash{}");

  return result;
}
