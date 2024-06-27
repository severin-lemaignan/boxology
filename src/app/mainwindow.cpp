/* See LICENSE file for copyright and license details. */

#include <qfiledialog.h>
#define DEBUG(x)                                                      \
    do {                                                              \
        std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] " << x; \
    } while (0)

#include <QBrush>
#include <QButtonGroup>
#include <QColor>
#include <QColorDialog>
#include <QFileDialog>
#include <QFont>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPen>
#include <QPushButton>
#include <QResizeEvent>
#include <QStatusBar>
#include <QString>
#include <QSvgGenerator>
#include <QTextEdit>
#include <fstream>
#include <iostream>

#include "../json/json.h"

// node editor
#include "../json_visitor.hpp"
#include "../md_visitor.hpp"
#include "../ros_visitor.hpp"
#include "../tikz_visitor.hpp"
#include "../view/cogbutton.hpp"
#include "../view/edge.hpp"
#include "../view/graphicsnode.hpp"
#include "../view/scene.hpp"
#include "../view/view.hpp"
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

    // create and configure scene
    _root_scene =
        make_shared<GraphicsNodeScene>(_root_arch.get(), nullptr, this);

    _root_scene->setSceneRect(-32000, -32000, 64000, 64000);

    //  view setup
    _view = make_shared<GraphicsNodeView>(this);

    set_active_scene(_root_scene.get());

    this->setCentralWidget(_view.get());

    // Prepare the cognitive functions buttons in the toolbar
    for (const auto& kv : LABEL_NAMES) {
        auto grp = new CogButton(kv.first);
        ui->cognitionToolbar->addWidget(grp);

        connect(grp, &CogButton::triggered, this,
                &MainWindow::onCogButtonTriggered);
    }

    spawnInitialNodes();

    ui->statusBar->showMessage(
        QString::fromStdString(hierarchy_name("", _root_scene.get())));
}

MainWindow::~MainWindow() { delete ui; }

string MainWindow::hierarchy_name(const string& name,
                                  GraphicsNodeScene* scene) {
    if (!scene->parent_node) return name;
    return hierarchy_name(name, dynamic_cast<GraphicsNodeScene*>(
                                    scene->parent_node->scene())) +
           " > " + scene->architecture->name;
}

void MainWindow::set_active_scene(GraphicsNodeScene* scene) {
    ui->statusBar->showMessage(
        QString::fromStdString(hierarchy_name("", scene)));
    _active_scene = scene;
    _view->setScene(scene);
    _active_arch = scene->architecture;
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
}

void MainWindow::spawnInitialNodes() {
    auto node = _active_arch->createNode();
    node->name("Node 1");
    node->label(Label::SOC_PERCEPTION);
    auto p1 = node->createPort(
        {"output", Port::Direction::OUT, Port::Type::EXPLICIT});
    auto gNode = _root_scene->add(node);
    gNode->setPos(30, 170);

    auto node2 = _active_arch->createNode();
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

void MainWindow::save(const std::string& filename) {
    JsonVisitor json(*_root_arch.get());
    auto output = json.visit();

    ofstream json_file(filename, std::ofstream::out);

    json_file << output;

    _root_arch->filename = filename;

    ui->statusBar->showMessage("Saved to " + QString::fromStdString(filename),
                               3000);

    setWindowTitle(QCoreApplication::applicationName() + " - " +
                   QString::fromStdString(filename));
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
    if (!_root_arch->filename.empty()) {
        save(_root_arch->filename);
    } else {
        on_actionToJsonAs_triggered();
    }
}

void MainWindow::on_actionToJsonAs_triggered() {
    auto newPath = QFileDialog::getSaveFileName(
        this, "Save architecture to json", _jsonPath, "JSON file (*.json)");

    if (newPath.isEmpty()) return;

    _jsonPath = newPath;
    save(_jsonPath.toStdString());
}

void MainWindow::on_actionFromJson_triggered() {
    auto filename = QFileDialog::getOpenFileName(
        this, "Select an architecture to open", "", "*.json");

    if (filename.isNull()) return;

    load(filename.toStdString());
}

void MainWindow::load(const string& filename) {
    try {
        auto toaddtoremove = _root_arch->load(filename);
        auto newstuff = toaddtoremove.first;
        auto killedstuff = toaddtoremove.second;

        _active_scene->set_description(_root_arch->name, _root_arch->version,
                                       _root_arch->description);

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
        QMessageBox::warning(
            0, "Error while loading an architecture",
            QString::fromStdString(
                string("Unable to load the architecture from ") + filename +
                ":\n\nJSON syntax error."));
        return;
    } catch (runtime_error e) {
        QMessageBox::warning(
            0, "Error while loading an architecture",
            QString::fromStdString(
                string("Unable to load the architecture from ") + filename +
                ":\n\n" + e.what()));
        return;
    }
}

void MainWindow::onCogButtonTriggered(Label label) {
    for (auto node : _active_scene->selected()) {
        node->node().lock()->label(label);
    }
}

void MainWindow::on_actionSave_to_SVG_triggered() {
    QString newPath = QFileDialog::getSaveFileName(0, tr("Save SVG"), _svgPath,
                                                   tr("SVG files (*.svg)"));

    if (newPath.isEmpty()) return;

    _svgPath = newPath;
    saveSvg(_svgPath);
}

void MainWindow::saveSvg(const QString& path) const {
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
    QString newPath = QFileDialog::getSaveFileName(0, tr("Save PNG"), _pngPath,
                                                   tr("PNG files (*.png)"));

    if (newPath.isEmpty()) return;

    _pngPath = newPath;
    savePng(_pngPath);
}

void MainWindow::savePng(const QString& path) const {
    const int RESOLUTION = 450;  // dpi
    auto width = _view->visibleRegion().boundingRect().width() /
                 _view->physicalDpiX() * RESOLUTION;
    auto height = _view->visibleRegion().boundingRect().height() /
                  _view->physicalDpiY() * RESOLUTION;
    QImage image(width, height, QImage::Format_ARGB32);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    _view->hideHelpers();
    _view->render(&painter);
    _view->showHelpers();

    image.save(path);
}

void MainWindow::on_actionExport_to_TikZ_triggered() {
    QString newPath = QFileDialog::getSaveFileName(
        nullptr, tr("Export to TikZ"), _tikzPath, tr("TikZ files (*.tex)"));

    if (newPath.isEmpty()) return;

    _tikzPath = newPath;
    saveTikZ(_tikzPath.toStdString());
}
void MainWindow::saveTikZ(const std::string& filename) const {
    TikzVisitor tikz(*_root_arch.get());
    auto output = tikz.visit();

    ofstream tikz_file(filename, std::ofstream::out);

    tikz_file << output;
}

void MainWindow::on_actionExport_to_Md_triggered() {
    QString newPath = QFileDialog::getExistingDirectory(
        nullptr, tr("Choose path for Markdown export"), _rosWsPath);

    if (newPath.isEmpty()) return;

    _mdPath = newPath;
    saveMd(_mdPath.toStdString());
}

void MainWindow::on_actionExport_to_Ros_triggered() {
    QString newPath = QFileDialog::getExistingDirectory(
        nullptr, tr("Choose ROS workspace path"), _rosWsPath);

    if (newPath.isEmpty()) return;

    _rosWsPath = newPath;
    exportRos(_rosWsPath.toStdString());
}

void MainWindow::saveMd(const std::string& path) const {
    MdVisitor md(*_root_arch.get(), path);
    md.visit();
}

void MainWindow::exportRos(const std::string& path) const {
    RosVisitor ros(*_root_arch.get(), path);

    ros.visit();
}

