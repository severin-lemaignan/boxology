/* See LICENSE file for copyright and license details. */

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

#include <iostream>


// node editor
#include "../graphicsnodescene.hpp"
#include "../graphicsnodeview.hpp"
#include "../graphicsnode.hpp"
#include "../graphicsbezieredge.hpp"

#include "../json_visitor.hpp"

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "test_qobjects.hpp"

using namespace std;

MainWindow::MainWindow()
  : arch(new Architecture),
    ui(new Ui::MainWindow),
    _view(nullptr),
    _scene(nullptr)
{

    ui->setupUi(this);

    // create and configure scene
    _scene = make_shared<GraphicsNodeScene>(arch.get(), this);

    _scene->setSceneRect(-32000, -32000, 64000, 64000);

    //  view setup
    _view = make_shared<GraphicsNodeView>(this);
    _view->setScene(_scene.get());
    this->setCentralWidget(_view.get());


    connect(_scene.get(), &GraphicsNodeScene::nodeDeleted,
            this, &MainWindow::onNodeDeleted);

    // add some content
    //addFakeContent();
    addNodeViews();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::
resizeEvent(QResizeEvent *event)
{
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

void MainWindow::addNodeViews()
{
    auto node = arch->createNode();
    node->name("NLP");
    auto p1 = node->createPort({"output", Port::Direction::OUT, Port::Type::EXPLICIT});
    auto gNode = _scene->add(node);
    gNode->setPos(0,0);


    auto node2 = arch->createNode();
    node2->name("KB");
    auto p2 = node2->createPort({"input", Port::Direction::IN, Port::Type::EXPLICIT});
    auto gNode2 = _scene->add(node2);
    gNode2->setPos(0+gNode->width()*1.5,0);

    node2->createPort({"output", Port::Direction::OUT, Port::Type::EXPLICIT});
    node2->createPort({"input2", Port::Direction::IN, Port::Type::LATENT});
    

    auto conn = arch->createConnection(node, p1, node2, p2);
    _scene->add(conn);


    //arch.addConnection(*e12);
}


void MainWindow::on_actionAdd_node_triggered()
{

    auto node = arch->createNode();

    node->name("new node");
    node->createPort({"input", Port::Direction::IN, Port::Type::EXPLICIT});
    node->createPort({"output", Port::Direction::OUT, Port::Type::EXPLICIT});

    auto gNode = _scene->add(node);
    gNode->setPos(0,0);


}

void MainWindow::on_actionToJson_triggered()
{

    JsonVisitor json(*arch);
    json.visit();

}


void MainWindow::onNodeAdded(NodePtr node)
{
    arch->addNode(node);
}
void MainWindow::onNodeDeleted(NodePtr node)
{
    arch->removeNode(node);
}

void MainWindow::onConnectionAdded(ConnectionPtr connection)
{
    arch->addConnection(connection);
}
void MainWindow::onConnectionDeleted(ConnectionPtr connection)
{
    arch->removeConnection(connection);
}

