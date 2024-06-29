/* See LICENSE file for copyright and license details. */

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFileInfo>
#include <iostream>

#include "../inja_visitor.hpp"
#include "../json/json.h"
#include "../json_visitor.hpp"
#include "../md_visitor.hpp"
#include "../ros_visitor.hpp"
#include "../rst_visitor.hpp"
#include "../tikz_visitor.hpp"
#include "mainwindow.hpp"

using namespace std;

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QCoreApplication::setApplicationName("boxology");
  QCoreApplication::setApplicationVersion(STR(BOXOLOGY_VERSION));

  QCommandLineParser parser;
  parser.setApplicationDescription(
      "A tool to model cognitive architectures, with a focus on robotics.");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument(
      "model", "The model to open/process (Boxology JSON format)");

  parser.addOptions(
      {{{"j", "to-json"}, "Export the model to JSON, without opening the GUI"},
       {{"t", "tpl"},
        "Export the model using a custom Inja template, without opening the "
        "GUI",
        "template"},
       {{"m", "to-markdown"},
        "Export the model to Markdown, without opening the GUI"},
       {{"l", "to-latex"}, "Export the model to a standalone LaTex (Tikz)"},
       {{"s", "to-rst"},
        "Export the architecture as a reStructured/Sphinx project",
        "documentation root"},
       {{"r", "to-ros"},
        "Export the architecture to a ROS workspace",
        "workspace root"}});

  // Process the actual command line arguments given by the user
  parser.process(app);

  auto args = parser.positionalArguments();
  if (args.empty()) {
    MainWindow win;
    win.show();
    return app.exec();
  } else {
    if (parser.isSet("to-json") || parser.isSet("tpl") ||
        parser.isSet("to-markdown") || parser.isSet("to-latex") ||
        parser.isSet("to-ros") || parser.isSet("to-rst")) {

      auto architecture = Architecture();

      try {
        architecture.load(args.at(0).toStdString());
      } catch (Json::RuntimeError jre) {
        cerr << "Unable to process the architecture: invalid JSON!";
        return 1;
      } catch (runtime_error e) {
        cerr << "Unable to process the architecture:" << e.what();
        return 1;
      }

      if (parser.isSet("to-json")) {
        JsonVisitor json(architecture);
        auto output = json.visit();

        cout << output;

        return 0;
      } else if (parser.isSet("tpl")) {
        QFileInfo src(args[0]);
        QFileInfo tpl(parser.values("tpl")[0]);

        auto output_file =
            src.absolutePath() + "/" + src.baseName() + "." + tpl.suffix();

        InjaVisitor visitor(architecture, parser.values("tpl")[0].toStdString(),
                            output_file.toStdString());

        if (visitor.ready()) {
          visitor.visit();
          return 0;
        } else {
          return 1;
        }

      } else if (parser.isSet("to-markdown")) {
        QFileInfo src(args[0]);
        MdVisitor visitor(architecture, src.absolutePath().toStdString());
        auto output = visitor.visit();

        cout << output;

        return 0;
      } else if (parser.isSet("to-latex")) {
        TikzVisitor tikz(architecture);
        auto output = tikz.visit();

        cout << output;

        return 0;
      } else if (parser.isSet("to-rst")) {
        QFileInfo src(parser.value("to-rst"));
        RstVisitor visitor(architecture, src.absolutePath().toStdString());
        auto output = visitor.visit();

        cout << output;

        return 0;
      } else if (parser.isSet("to-ros")) {
        QFileInfo src(parser.value("to-ros"));
        RosVisitor visitor(architecture, src.absolutePath().toStdString());
        auto output = visitor.visit();

        cout << output;

        return 0;
      }
    } else {
      MainWindow win;
      win.load(args.at(0).toStdString());
      win.show();
      return app.exec();
    }
  }
}
