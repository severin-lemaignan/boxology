/* See LICENSE file for copyright and license details. */

#define DEBUG(x)                                                      \
    do {                                                              \
        std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] " << x; \
    } while (0)

#include <QPen>
#include <QColor>
#include <QFont>
#include <QBrush>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QColorDialog>
#include <QPushButton>
#include <QTextEdit>
#include <QStatusBar>
#include <QResizeEvent>
#include <QButtonGroup>
#include <QString>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>

#include <iostream>
#include <fstream>
#include <json/json.h>

// node editor
#include "../view/cogbutton.hpp"
#include "../view/scene.hpp"
#include "../view/view.hpp"
#include "../view/graphicsnode.hpp"
#include "../view/edge.hpp"

#include "../json_visitor.hpp"

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "test_qobjects.hpp"

using namespace std;

MainWindow::MainWindow()
    : _root_arch(new Architecture),
      _active_arch(_root_arch.get()),
      ui(new Ui::MainWindow),
      _view(nullptr),
      _scene(nullptr) {

    ui->setupUi(this);

    // create and configure scene
    _scene = make_shared<GraphicsNodeScene>(_root_arch.get(), nullptr, this);

    _scene->setSceneRect(-32000, -32000, 64000, 64000);

    //  view setup
    _view = make_shared<GraphicsNodeView>(this);
    _view->setScene(_scene.get());
    this->setCentralWidget(_view.get());

    // Prepare the cognitive functions buttons in the toolbar
    for (const auto& kv : COGNITIVE_FUNCTION_NAMES) {
        auto grp = new CogButton(kv.first);
        ui->cognitionToolbar->addWidget(grp);

        connect(grp, &CogButton::triggered, this,
                &MainWindow::onCogButtonTriggered);
    }

    spawnInitialNodes();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
}

void MainWindow::spawnInitialNodes() {
    auto node = _root_arch->createNode();
    node->name("Node 1");
    node->cognitive_function(CognitiveFunction::PERCEPTION);
    auto p1 = node->createPort(
        {"output", Port::Direction::OUT, Port::Type::EXPLICIT});
    auto gNode = _scene->add(node);
    gNode->setPos(30, 170);

    auto node2 = _root_arch->createNode();
    node2->name("Node 2");
    auto p2 =
        node2->createPort({"input", Port::Direction::IN, Port::Type::EXPLICIT});
    auto gNode2 = _scene->add(node2);
    gNode2->setPos(30 + gNode->width() * 2, 170);

    node2->createPort({"output", Port::Direction::OUT, Port::Type::EXPLICIT});

    auto conn = _root_arch->createConnection({node, p1}, {node2, p2});
    conn->name = "data";
    _scene->add(conn);
}

void MainWindow::save(const std::string& filename) const {

    JsonVisitor json(*_active_arch);
    auto output = json.visit();

    ofstream json_file(filename, std::ofstream::out);

    json_file << output;

}

void MainWindow::on_actionAdd_node_triggered() {
    auto node = _active_arch->createNode();

    node->name("Node");
    node->createPort({"input", Port::Direction::IN, Port::Type::EXPLICIT});
    node->createPort({"output", Port::Direction::OUT, Port::Type::EXPLICIT});

    auto gNode = _scene->add(node);
    gNode->setPos(0, 0);
}

void MainWindow::on_actionToJson_triggered() {

    auto filename = QFileDialog::getSaveFileName(
        this, "Save architecture to json", "", "*.json");

    save(filename.toStdString());

}

void MainWindow::on_actionFromJson_triggered() {
    Json::Value root;

    auto filename = QFileDialog::getOpenFileName(
        this, "Select an architecture to open", "", "*.json");

    if (filename.isNull()) return;

    ifstream json_file(filename.toStdString());

    try {
        json_file >> root;
    } catch (Json::RuntimeError jre) {
        QMessageBox::warning(0, "Error while loading an architecture",
                QString::fromStdString(string("Unable to load the architecture from ") +
                filename.toStdString() + ":\n\nJSON syntax error."));
        return;
    }


    // 'Dummy' load to make sure the JSON is valid, without impacting the current arch
    // -> prevent going in a 'semi-loaded' state
    try {
        Architecture* new_arch{new Architecture()};
        new_arch->load(root);
    }
    catch (runtime_error e) {
        QMessageBox::warning(0, "Error while loading an architecture",
                QString::fromStdString(string("Unable to load the architecture from ") +
                filename.toStdString() + ":\n\n" + e.what()));
        return;
    }

    auto newstuff = _active_arch->load(root);

    _scene->set_description(_active_arch->name, _active_arch->version, _active_arch->description);

    DEBUG("Loaded " << newstuff.first.size() << " nodes and "
            << newstuff.second.size() << " connections." << endl);

    for (auto n : newstuff.first) {
        _scene->add(n);
    }
    for (auto c : newstuff.second) {
        _scene->add(c);
    }
}

void MainWindow::onCogButtonTriggered(CognitiveFunction cognitive_function) {
    for (auto node : _scene->selected()) {
        node->node().lock()->cognitive_function(cognitive_function);
    }
}
