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
    : arch(new Architecture),
      ui(new Ui::MainWindow),
      _view(nullptr),
      _scene(nullptr) {
    ui->setupUi(this);

    // create and configure scene
    _scene = make_shared<GraphicsNodeScene>(arch.get(), nullptr, this);

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

    // add some content
    // addFakeContent();
    addNodeViews();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
}

/*
void MainWindow::
addFakeContent()
{
    // fill with some content
    QBrush greenBrush(Qt::green);
    QPen outlinePen(Qt::black);
    outlinePen.setWidth(2);

    // populate with a of lines
    auto gridColor = QColor::fromRgbF(0.4, 0.4, 0.4, 1.0);
    QBrush gridBrush(gridColor);
    QPen gridPen(gridColor);

    // populate with 'content'
    auto rect = _scene->addRect(100, 0, 80, 100, outlinePen, greenBrush);
    rect->setFlag(QGraphicsItem::ItemIsMovable);

    auto text = _scene->addText("scene01", QFont("Ubuntu Mono"));
    text->setDefaultTextColor(QColor::fromRgbF(1.0, 1.0, 1.0, 1.0));
    text->setFlag(QGraphicsItem::ItemIsMovable);
    text->setPos(0, -25);

    auto widget1 = new QPushButton("Hello World");
    auto proxy1 = _scene->addWidget(widget1);
    proxy1->setPos(0, 30);

    auto widget2 = new QTextEdit();
    auto proxy2 = _scene->addWidget(widget2);
    proxy2->setPos(0, 60);
}
*/

void MainWindow::addNodeViews() {
    auto node = arch->createNode();
    node->name("Node 1");
    node->cognitive_function(CognitiveFunction::PERCEPTION);
    auto p1 = node->createPort(
        {"output", Port::Direction::OUT, Port::Type::EXPLICIT});
    auto gNode = _scene->add(node);
    gNode->setPos(30, 170);

    auto node2 = arch->createNode();
    node2->name("Node 2");
    auto p2 =
        node2->createPort({"input", Port::Direction::IN, Port::Type::EXPLICIT});
    auto gNode2 = _scene->add(node2);
    gNode2->setPos(30 + gNode->width() * 2, 170);

    node2->createPort({"output", Port::Direction::OUT, Port::Type::EXPLICIT});

    auto conn = arch->createConnection({node, p1}, {node2, p2});
    conn->name = "data";
    _scene->add(conn);
}

void MainWindow::save(const std::string& filename) const {

    JsonVisitor json(*arch);
    auto output = json.visit();

    ofstream json_file(filename, std::ofstream::out);

    json_file << output;

}

void MainWindow::on_actionAdd_node_triggered() {
    auto node = arch->createNode();

    node->name("new node");
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

    Architecture* new_arch{new Architecture()};

    try {
        auto newstuff = new_arch->load(root);
        arch.reset(new_arch);

        _scene->set_description(arch->name, arch->version, arch->description);

        DEBUG("Loaded " << newstuff.first.size() << " nodes and "
                << newstuff.second.size() << " connections." << endl);

        for (auto n : newstuff.first) {
            _scene->add(n);
        }
        for (auto c : newstuff.second) {
            _scene->add(c);
        }
    }
    catch (runtime_error e) {
        QMessageBox::warning(0, "Error while loading an architecture",
                QString::fromStdString(string("Unable to load the architecture from ") +
                filename.toStdString() + ":\n\n" + e.what()));
        return;
    }
}

void MainWindow::onCogButtonTriggered(CognitiveFunction cognitive_function) {
    for (auto node : _scene->selected()) {
        node->node().lock()->cognitive_function(cognitive_function);
    }
}
