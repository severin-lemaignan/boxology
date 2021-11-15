/* See LICENSE file for copyright and license details. */

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <iostream>

#include "../architecture_manager.hpp"
#include "../json/json.h"
#include "../json_visitor.hpp"
#include "../md_visitor.hpp"
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

    parser.addOptions({{{"l", "list-models"}, "Lists available models"}});

    parser.addOptions({{{"j", "to-json"},
                        "Export the model to JSON, without opening the GUI"}});
    parser.addOptions(
        {{{"m", "to-markdown"},
          "Export the model to Markdown, without opening the GUI"}});

    parser.addOptions({{{"t", "to-latex"},
                        "Export the model to a standalone LaTex (Tikz) "
                        "document, without opening the GUI"}});

    // Process the actual command line arguments given by the user
    parser.process(app);

    if (parser.isSet("list-models")) {
        auto manager = ArchitectureManager();
        manager.list_models();
        return 0;
    }

    auto args = parser.positionalArguments();
    if (args.empty()) {
        MainWindow win;
        win.show();
        return app.exec();
    } else {
        if (parser.isSet("to-json") || parser.isSet("to-markdown") ||
            parser.isSet("to-latex")) {
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
            } else if (parser.isSet("to-markdown")) {
                MdVisitor md(architecture);
                auto output = md.visit();

                cout << output;

                return 0;
            } else if (parser.isSet("to-latex")) {
                TikzVisitor tikz(architecture);
                auto output = tikz.visit();

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
