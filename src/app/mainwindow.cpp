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
#include <QSvgGenerator>

#include <iostream>
#include <fstream>
#include "../json/json.h"

// node editor
#include "../view/cogbutton.hpp"
#include "../view/scene.hpp"
#include "../view/view.hpp"
#include "../view/graphicsnode.hpp"
#include "../view/edge.hpp"

#include "../json_visitor.hpp"
#include "../tikz_visitor.hpp"

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow()
    : _root_arch(new Architecture),
      _active_arch(_root_arch.get()),
      ui(new Ui::MainWindow),
      _view(nullptr),
      _root_scene(nullptr),
      _active_scene(nullptr) {

    ui->setupUi(this);

    manager.list_models();

    // create and configure scene
    _root_scene = make_shared<GraphicsNodeScene>(_root_arch.get(), nullptr, this);


    _root_scene->setSceneRect(-32000, -32000, 64000, 64000);

    //  view setup
    _view = make_shared<GraphicsNodeView>(this);

    set_active_scene(_root_scene.get());

    this->setCentralWidget(_view.get());

    // Prepare the cognitive functions buttons in the toolbar
    for (const auto& kv : COGNITIVE_FUNCTION_NAMES) {
        auto grp = new CogButton(kv.first);
        ui->cognitionToolbar->addWidget(grp);

        connect(grp, &CogButton::triggered, this,
                &MainWindow::onCogButtonTriggered);
    }

    spawnInitialNodes();

    ui->statusBar->showMessage(QString::fromStdString(hierarchy_name("", _root_scene.get())));
}

MainWindow::~MainWindow() { delete ui; }

string MainWindow::hierarchy_name(const string& name, GraphicsNodeScene* scene) {
    if (!scene->parent_node) return name;
    return  hierarchy_name(name, dynamic_cast<GraphicsNodeScene*>(scene->parent_node->scene())) + " > " + scene->architecture->name;
}

void MainWindow::set_active_scene(GraphicsNodeScene* scene) {

    ui->statusBar->showMessage(QString::fromStdString(hierarchy_name("", scene)));
    _active_scene = scene;
    _view->setScene(scene);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
}

void MainWindow::spawnInitialNodes() {
    auto node = _root_arch->createNode();
    node->name("Node 1");
    node->cognitive_function(CognitiveFunction::PERCEPTION);
    auto p1 = node->createPort(
        {"output", Port::Direction::OUT, Port::Type::EXPLICIT});
    auto gNode = _root_scene->add(node);
    gNode->setPos(30, 170);

    auto node2 = _root_arch->createNode();
    node2->name("Node 2");
    auto p2 =
        node2->createPort({"input", Port::Direction::IN, Port::Type::EXPLICIT});
    auto gNode2 = _root_scene->add(node2);
    gNode2->setPos(30 + gNode->width() * 2, 170);

    node2->createPort({"output", Port::Direction::OUT, Port::Type::EXPLICIT});

    auto conn = _root_arch->createConnection({node, p1}, {node2, p2});
    conn->name = "data";
    _root_scene->add(conn);
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

    auto gNode = _active_scene->add(node);
    gNode->setPos(0, 0);
}

void MainWindow::on_actionToJson_triggered() {

    auto newPath = QFileDialog::getSaveFileName(
        this, "Save architecture to json", _jsonPath, "JSON file (*.json)");

    if (newPath.isEmpty())
        return;

    _jsonPath = newPath;
    save(_jsonPath.toStdString());

}

void MainWindow::on_actionFromJson_triggered() {

    auto filename = QFileDialog::getOpenFileName(
        this, "Select an architecture to open", "", "*.json");

    if (filename.isNull()) return;


    try {
        auto toaddtoremove = _active_arch->load(filename.toStdString());
        auto newstuff = toaddtoremove.first;
        auto killedstuff = toaddtoremove.second;

        _active_scene->set_description(_active_arch->name, _active_arch->version, _active_arch->description);

        for (auto n : killedstuff.first) {
            _active_scene->remove(n);
        }

        DEBUG("Loaded " << newstuff.first.size() << " nodes and "
                << newstuff.second.size() << " connections." << endl);

        for (auto n : newstuff.first) {
            _active_scene->add(n);
        }
        for (auto c : newstuff.second) {
            _active_scene->add(c);
        }
    } catch (Json::RuntimeError jre) {
        QMessageBox::warning(0, "Error while loading an architecture",
                QString::fromStdString(string("Unable to load the architecture from ") +
                    filename.toStdString() + ":\n\nJSON syntax error."));
        return;
    } catch (runtime_error e) {
        QMessageBox::warning(0, "Error while loading an architecture",
                QString::fromStdString(string("Unable to load the architecture from ") +
                    filename.toStdString() + ":\n\n" + e.what()));
        return;
    }


}

void MainWindow::onCogButtonTriggered(CognitiveFunction cognitive_function) {
    for (auto node : _active_scene->selected()) {
        node->node().lock()->cognitive_function(cognitive_function);
    }
}

void MainWindow::on_actionSave_to_SVG_triggered() {

    QString newPath = QFileDialog::getSaveFileName(0, tr("Save SVG"),
        _svgPath, tr("SVG files (*.svg)"));

    if (newPath.isEmpty())
        return;

    _svgPath = newPath;
    saveSvg(_svgPath);
}

void MainWindow::saveSvg(const QString& path) const
{

    QSvgGenerator generator;
    generator.setFileName(path);
    generator.setSize(_view->visibleRegion().boundingRect().size());
    generator.setViewBox(_view->visibleRegion().boundingRect());
    generator.setTitle(tr("Boxology diagram"));
    generator.setDescription(tr("A Boxology diagram"));
    QPainter painter(&generator);

    _view->hideHelpers();
    _view->disableGraphicsEffects();
    _view->render(&painter);
    _view->enableGraphicsEffects();
    _view->showHelpers();
}

void MainWindow::on_actionExport_to_PNG_triggered() {

    QString newPath = QFileDialog::getSaveFileName(0, tr("Save PNG"),
        _pngPath, tr("PNG files (*.png)"));

    if (newPath.isEmpty())
        return;

    _pngPath = newPath;
    savePng(_pngPath);
}


void MainWindow::savePng(const QString& path) const
{

    const int RESOLUTION = 300; //dpi
    auto width = _view->visibleRegion().boundingRect().width() / _view->physicalDpiX() * RESOLUTION;
    auto height = _view->visibleRegion().boundingRect().height() / _view->physicalDpiY() * RESOLUTION;
    QImage image(width,height,QImage::Format_ARGB32);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    _view->hideHelpers();
    _view->render(&painter);
    _view->showHelpers();

    image.save(path);
}

void MainWindow::on_actionExport_to_TikZ_triggered()
{

    QString newPath = QFileDialog::getSaveFileName(0, tr("Export to TikZ"),
        _tikzPath, tr("TikZ files (*.tex)"));

    if (newPath.isEmpty())
        return;

    _tikzPath = newPath;
    saveTikZ(_tikzPath.toStdString());

}

void MainWindow::saveTikZ(const std::string& filename) const {

    TikzVisitor tikz(*_active_arch);
    auto output = tikz.visit();

    ofstream tikz_file(filename, std::ofstream::out);

    tikz_file << output;

}
