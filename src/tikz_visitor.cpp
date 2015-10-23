#include <algorithm>
#include <string>

#include "tikz_visitor.hpp"

#include "cognitive_function.hpp"

using namespace std;

void TikzVisitor::startUp() {
    ss << "\\documentclass{article}" << endl;
    ss << endl;
    ss << "\\usepackage{tikz}" << endl;
    ss << "\\usetikzlibrary{positioning}" << endl;
    ss << endl;
    ss << endl;

    // Generate colors based on what is used for Boxology's GUI
    for(const auto& kv : COGNITIVE_FUNCTION_COLORS) {
        auto color = kv.second;
        transform(color.begin(), color.end(), color.begin(), ::toupper);

        ss << "\\definecolor{" << make_tex_id(COGNITIVE_FUNCTION_NAMES.at(kv.first)) <<
               "}{HTML}{" << color.substr(1) << "}" << endl;
    }
    
    ss << "\n\\begin{document}" << endl;
    ss << "    \\begin{figure}" << endl;
    ss << "    \\centering" << endl;
    ss << endl;

    ss << "    \\resizebox{\\textwidth}{!}{%" << endl;
    ss << "    \\begin{tikzpicture}[" << endl;
    ss << "                 font=\\sffamily," << endl;
    ss << "                 >=latex," << endl;
    ss << "                 every edge/.style={draw, very thick}," << endl;
    ss << "                 node/.style={draw, rounded corners, align=center, inner sep=5pt, fill=black!20}," << endl;
    ss << "                 label/.style={midway, align=center, "
          "font=\\scriptsize\\sffamily, fill=white, text width=20mm}]" << endl;
    ss << endl;


}

void TikzVisitor::tearDown() {
    ss << endl;
    ss << "    \\end{tikzpicture}" << endl;
    ss << "    }" << endl;
    ss << "    \\label{fig:" << make_tex_id(architecture.name) << "}" << endl;
    ss << "    \\caption{The {\\sc " << architecture.name << "} architecture}" << endl;
    ss << "    \\end{figure}" << endl;
    ss << "\\end{document}" << endl;

    _content = ss.str();
}

void TikzVisitor::beginNodes() {
    ss << "        %%% NODES" << endl;
}

void TikzVisitor::onNode(shared_ptr<const Node> node) {

    ss << "        \\node "
          "at (" << tikz_unit(node->x()) << "," << tikz_unit(-node->y()) << ") "
          "[node, "
          "minimum width=" << tikz_unit(node->width()) << ", "
          "minimum height=" << tikz_unit(node->height()) << ", "
          "fill=" << make_tex_id(COGNITIVE_FUNCTION_NAMES.at(node->cognitive_function())) << "!50]" <<
          " (" << make_tex_id(node->name()) << ") {" << node->name() << "};";
    ss << endl;

    //for (const auto port : node->ports()) {
    //    port->name;
    //    (port->direction == Port::Direction::IN ? "in" : "out");
    //}
}

void TikzVisitor::beginConnections() {
    ss << endl;
    ss << "        %%% CONNECTIONS" << endl;
}

void TikzVisitor::onConnection(shared_ptr<const Connection> connection) {
    //connection->name;

    auto from = connection->from.node.lock();
    auto to = connection->to.node.lock();

    ss << "        \\path (" << make_tex_id(from->name()) << ") "
           "edge [->, bend right] node[label] {" << connection->name << "}"
           "(" << make_tex_id(to->name()) << ");" << endl;

}


string TikzVisitor::tikz_unit(const double dim) {

    stringstream ss;
    ss << (int) (dim * scale) <<"mm";
    return ss.str();
 
}

string TikzVisitor::make_tex_id(const std::string& name) {

    string result;

    for (const char& c : name) {

        switch(c) {

        case ' ':
        case '.':
        case '-':
        case '\\':
        case '[':
        case ']':
        case '{':
        case '}':
        case '(':
        case ')':
        case '<':
        case '>':
            break;
        default:
            result += tolower(c);
        }
    }

    return result;
}
