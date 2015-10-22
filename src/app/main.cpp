/* See LICENSE file for copyright and license details. */

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#include <iostream>
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include "json/json.h"

#include "mainwindow.hpp"
#include "../json_visitor.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("Boxology");
    QCoreApplication::setApplicationVersion(STR(BOXOLOGY_VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription("A tool to model cognitive architectures, with a focus on robotics."
                                     "\n\nThe default cmd-line behaviour is to fully expand the given \n"
                                     "architecture and print the result as JSON."
                                     "\n\nRun the application with no arguments to start the GUI.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("architecture", "The architecture to process (json file)");

    // Process the actual command line arguments given by the user
    parser.process(app);

    auto args = parser.positionalArguments();
    if(args.empty()) {
        MainWindow win;
        win.show();
        return app.exec();
    }
    else {
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

        JsonVisitor json(architecture);
        auto output = json.visit();

        cout << output;

        return 0;

    }
}
