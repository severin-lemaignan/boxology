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
  : ui(new Ui::MainWindow),
    _view(nullptr),
    _scene(nullptr)
{

    ui->setupUi(this);

    // create and configure scene
    _scene = make_shared<GraphicsNodeScene>(this);

    _scene->setSceneRect(-32000, -32000, 64000, 64000);

    //  view setup
    _view = make_shared<GraphicsNodeView>(this);
    _view->setScene(_scene.get());
    this->setCentralWidget(_view.get());


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

void MainWindow::
addNodeViews()
{
    /*
    for (int i = 0; i < 5; i++) {
        auto n = new GraphicsNode(nullptr);
        for (int j = i; j < 5; j++) {
            n->setPos(i * 25, i * 25);
            n->add_sink("sink");
            n->add_source("source");
        }

        if (i == 4) {
            QTextEdit *te = new QTextEdit();
            n->setCentralWidget(te);
        }

        n->setTitle(QString("GraphicsNode %1").arg(i));

        _scene->addItem(n);
    }
    */

    auto node = arch.createNode();
    node->name("NLP");
    node->addPort({"output", Port::Direction::OUT, Port::Type::EXPLICIT});
    auto gNode = _scene->addNode(node);
    gNode->setPos(0,0);


    auto node2 = arch.createNode();
    node2->name("KB");
    node2->addPort({"input", Port::Direction::IN, Port::Type::EXPLICIT});
    auto gNode2 = _scene->addNode(node2);
    gNode2->setPos(0+gNode->width()*1.5,0);

    node2->addPort({"output", Port::Direction::OUT, Port::Type::EXPLICIT});
    

    //auto conn = arch.createConnection(node, node2);

    //auto e12 = make_shared<GraphicsBezierEdge>();
    //e12->connect(n1,0,n2,0);
    //_scene->addItem(e12.get());

    //arch.addConnection(*e12);
}


void MainWindow::on_actionAdd_node_triggered()
{

    auto node = arch.createNode();

    node->name("new node");
    node->addPort({"input", Port::Direction::IN, Port::Type::EXPLICIT});
    node->addPort({"output", Port::Direction::OUT, Port::Type::EXPLICIT});

    auto gNode = _scene->addNode(node);
    gNode->setPos(0*1.5,0);


}

void MainWindow::on_actionToJson_triggered()
{

    JsonVisitor json(arch);
    json.visit();

}
