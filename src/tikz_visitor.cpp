#include "tikz_visitor.hpp"

#include <algorithm>
#include <boost/algorithm/string.hpp>  // for search and replace
#include <string>

#include "label.hpp"

using namespace std;

void TikzVisitor::startUp() {
    ss << "\\documentclass[tikz]{standalone}" << endl;
    ss << "%\\documentclass[tikz,convert=pdf2svg]{standalone} % generate a SVG "
          "file automatically"
       << endl;
    ss << endl;
    ss << "\\usetikzlibrary{positioning}" << endl;
    ss << endl;
    ss << endl;

    // Generate colors based on what is used for Boxology's GUI
    for (const auto& kv : LABEL_COLORS) {
        auto color = kv.second;
        transform(color.begin(), color.end(), color.begin(), ::toupper);

        auto[id, id_capitalized] = make_id(LABEL_NAMES.at(kv.first));
        ss << "\\definecolor{" << id << "}{HTML}{" << color.substr(1) << "}" << endl;
    }

    ss << "\n\\begin{document}" << endl;
    ss << endl;

    ss << "\\begin{tikzpicture}[" << endl;
    ss << "             font=\\sffamily," << endl;
    ss << "             >=latex," << endl;
    ss << "             every edge/.style={draw, very thick}," << endl;
    ss << "             node/.style={draw, rounded corners, align=center, "
          "inner sep=5pt, fill=black!20},"
       << endl;
    ss << "             label/.style={midway, align=center, "
          "fill=white, text width=20mm}]"
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

    auto[label_id, label_id_capitalized] = make_id(LABEL_NAMES.at(node->label()));
    auto[id, id_capitalized] = make_id(node->name());

    ss << "        \\node "
          "at ("
       << tikz_unit(node->x()) << "," << tikz_unit(-node->y())
       << ") "
          "[node, "
          "anchor=north west, "
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
       << label_id
       << "!50]"
       << " (" << id << ") {" << sanitize_tex(node->name())
       << "};";
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

    auto[from_id, from_id_capitalized] = make_id(from->name());
    auto[to_id, to_id_capitalized] = make_id(to->name());

    if (connection->name == "anonymous") {
        ss << "        \\path (" << from_id 
           << ") "
              "edge [->, out=0, in=180] "
              "("
           << to_id << ");" << endl;
    } else {
        ss << "        \\path (" << from_id
           << ") "
              "edge [->, out=0, in=180] node[label] {"
           << sanitize_tex(connection->name)
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

string TikzVisitor::sanitize_tex(const std::string& text) {
    string result(text);

    // special-case the backslash
    boost::replace_all(result, "\\", "\\textbackslash");

    map<string, string> tex_substitutions{
        {"{", "\\{"}, {"}", "\\}"},
        {"$", "\\$"}, {"&", "\\&"},
        {"#", "\\#"}, {"^", "\\textasciicircum{}"},
        {"_", "\\_"}, {"~", "\\textasciitilde{}"},
        {"%", "\\%"}};

    for (const auto& kv : tex_substitutions) {
        boost::replace_all(result, kv.first, kv.second);
    }

    // finish the backslash special-case
    boost::replace_all(result, "textbackslash", "textbackslash{}");

    return result;
}

