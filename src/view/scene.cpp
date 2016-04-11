/* See LICENSE file for copyright and license details. */

#include <cmath>
#include <QColor>
#include <QPainter>
#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QGraphicsView>
#include <QDebug>
#include <algorithm>
#include <iostream>

#include "scene.hpp"
#include "socket.hpp"
#include "../app/mainwindow.hpp"

using namespace std;

// TODO: move to graphicsnodeview. use graphicsnodescene for management

GraphicsNodeScene::GraphicsNodeScene(Architecture *architecture,
                                     GraphicsNode* parent_node,
                                     QObject *parent)
    : QGraphicsScene(parent),
      architecture(architecture),
      parent_node(parent_node),
      dontGrabKeyPresses(false),
      _paintBackground(true),
      _color_background(QColor("#393939")),
      _color_light(QColor("#2F2F2F")),
      _color_dark(QColor("#292929")),
      _color_null(QColor("#212121")),
      _color_bg_text(QColor("#a1a1a1")),
      _pen_light(QPen(_color_light)),
      _pen_dark(QPen(_color_dark)),
      _pen_null(QPen(_color_null)),
      _brush_background(_color_background),
      _arch_name(new EditableLabel()),
      _arch_version(new EditableLabel()),
      _arch_desc(new EditableDescription()) {
    // initialize default pen settings
    for (auto p : {&_pen_light, &_pen_dark, &_pen_null}) {
        p->setWidth(0);
    }

    // initialize the background
    setBackgroundBrush(_brush_background);

    // add text field with the architecture name
    _arch_name->setHtml("<h1>" + QString::fromStdString(architecture->name) + "</h1>");

    _arch_name->setPos(0, 0);
    _arch_name->setDefaultTextColor(_color_bg_text);

    this->addItem(_arch_name);
    connect(_arch_name, &EditableLabel::contentUpdated, this,
            &GraphicsNodeScene::onDescriptionChanged);

    // add text field with the architecture version
    _arch_version->setHtml("<h2>" + QString::fromStdString(architecture->version) + "</h2>");

    _arch_version->setPos(0, _arch_name->boundingRect().height());
    _arch_version->setDefaultTextColor(_color_bg_text);

    this->addItem(_arch_version);
    connect(_arch_version, &EditableLabel::contentUpdated, this,
            &GraphicsNodeScene::onDescriptionChanged);

    _arch_desc->setHtml(QString::fromStdString(architecture->description));

    _arch_desc->setTextWidth(400);
    _arch_desc->setPos(
        0, _arch_version->y() + _arch_version->boundingRect().height());
    _arch_desc->setDefaultTextColor(_color_null);

    this->addItem(_arch_desc);
    connect(_arch_desc, &EditableLabel::contentUpdated, this,
            &GraphicsNodeScene::onDescriptionChanged);
}

shared_ptr<GraphicsNode> GraphicsNodeScene::add(NodePtr node) {
    auto gNode = make_shared<GraphicsNode>(node);

    // connecting the node controller with the node view, so that
    // updates to the node controller are reflected in the widget.
    connect(node.get(), &Node::dirty, gNode.get(), &GraphicsNode::refreshNode);

    _nodes.insert(gNode);
    addItem(gNode.get());
    return gNode;
}

void GraphicsNodeScene::remove(std::shared_ptr<GraphicsNode> graphicNode) {

    auto node = graphicNode->node();
    if (node.expired()) {
        qWarning() << "Deleting a GraphicsNode fir an already deleted node!";
    }
    else {
        architecture->removeNode(node.lock());
    }

    graphicNode.get()->setSelected(false);
    graphicNode.get()->disconnect();

    _nodes.erase(graphicNode);

}

void GraphicsNodeScene::remove(NodePtr node) {

    set<shared_ptr<GraphicsNode>> to_remove;
    for(auto gn : _nodes) {
        if(!gn->node().expired() && gn->node().lock() == node) {
            to_remove.insert(gn);
        }
    }

    for(auto gn : to_remove) {
        remove(gn);
    }
}

shared_ptr<GraphicsDirectedEdge> GraphicsNodeScene::add(
    ConnectionPtr connection) {
    auto from = connection->from.node.lock();
    auto to = connection->to.node.lock();

    auto edge = make_edge();

    // look for the *graphic nodes* that represent the source/sink of
    // our connection:
    auto source = find_if(_nodes.begin(), _nodes.end(),
                          [&from](const shared_ptr<GraphicsNode> gNode) {
                              return gNode->node().lock() == from;
                          });

    auto sink = find_if(_nodes.begin(), _nodes.end(),
                        [&to](const shared_ptr<GraphicsNode> gNode) {
                            return gNode->node().lock() == to;
                        });

    // qWarning() << "Connecting " << QString::fromStdString(from->name()) << "
    // to " << QString::fromStdString(to->name());
    edge->connect((*source).get(), connection->from.port.lock().get(),
                  (*sink).get(), connection->to.port.lock().get());

    return edge;
}

shared_ptr<GraphicsDirectedEdge> GraphicsNodeScene::make_edge() {
    auto edge = make_shared<GraphicsBezierEdge>();

    connect(edge.get(), &GraphicsDirectedEdge::connectionEstablished, this,
            &GraphicsNodeScene::onConnectionEstablished);
    connect(edge.get(), &GraphicsDirectedEdge::connectionDisrupted, this,
            &GraphicsNodeScene::onConnectionDisrupted);

    _edges.insert(edge);
    addItem(edge.get());
    return edge;
}

void GraphicsNodeScene::onConnectionEstablished(
    shared_ptr<GraphicsDirectedEdge> edge) {
    auto conn = architecture->createConnection(edge->source()->socket(),
                                               edge->sink()->socket());

    edge->setUnderlyingConnection(conn);
}

void GraphicsNodeScene::onConnectionDisrupted(
    shared_ptr<GraphicsDirectedEdge> edge) {
    // qWarning() << "Connection disrupted!";
    architecture->removeConnection(edge->source()->socket(),
                                   edge->sink()->socket());
}

void GraphicsNodeScene::onDescriptionChanged(const QString &content) {
    architecture->name = _arch_name->toPlainText().toStdString();
    architecture->version = _arch_version->toPlainText().toStdString();
    architecture->description = _arch_desc->toHtml().toStdString();
}

void GraphicsNodeScene::hideHelpers()
{

    _paintBackground = false;

    for (auto node : _nodes) {
        node->hideHelpers();
    }
}

void GraphicsNodeScene::showHelpers()
{
    _paintBackground = true;

     for (auto node : _nodes) {
        node->showHelpers();
     }
}

void GraphicsNodeScene::disableGraphicsEffects()
{
     for (auto node : _nodes) {
        node->disableGraphicsEffects();
     }
     for (auto edge : _edges) {
        edge->disableGraphicsEffects();
     }

}
void GraphicsNodeScene::enableGraphicsEffects()
{
     for (auto node : _nodes) {
        node->enableGraphicsEffects();
     }
     for (auto edge : _edges) {
        edge->enableGraphicsEffects();
     }

}

set<shared_ptr<GraphicsNode>> GraphicsNodeScene::selected() const {
    set<shared_ptr<GraphicsNode>> selectedNodes;

    for (auto graphicNode : _nodes) {
        if (graphicNode->isSelected()) {
            selectedNodes.insert(graphicNode);
        }
    }
    return selectedNodes;
}

void GraphicsNodeScene::set_description(const string &name,
                                        const string &version,
                                        const string &desc) {
    _arch_name->setHtml(
        QString::fromStdString(string("<h1>") + name + "</h1>"));
    _arch_version->setHtml(
        QString::fromStdString(string("<h2>") + version + "</h2>"));
    _arch_desc->setHtml(QString::fromStdString(desc));
}

/*
 * TODO: move the visualization into the graphicsview, and move all the GUI
 * logic into the graphicsnodescene
 */
void GraphicsNodeScene::drawBackground(QPainter *painter, const QRectF &rect) {

    if(!_paintBackground) return;

    // call parent method
    QGraphicsScene::drawBackground(painter, rect);

    // augment the painted with grid
    const int gridsize = 20;
    auto left = static_cast<int>(std::floor(rect.left()));
    auto right = static_cast<int>(std::ceil(rect.right()));
    auto top = static_cast<int>(std::floor(rect.top()));
    auto bottom = static_cast<int>(std::ceil(rect.bottom()));

    // compute indices of lines to draw
    const auto first_left = left - (left % gridsize);
    const auto first_top = top - (top % gridsize);

    // compute lines to draw and
    std::vector<QLine> lines_light;
    std::vector<QLine> lines_dark;
    for (auto x = first_left; x <= right; x += gridsize) {
        if (x % 100 != 0)
            lines_light.push_back(QLine(x, top, x, bottom));
        else
            lines_dark.push_back(QLine(x, top, x, bottom));
    }
    for (auto y = first_top; y <= bottom; y += gridsize) {
        if (y % 100 != 0)
            lines_light.push_back(QLine(left, y, right, y));
        else
            lines_dark.push_back(QLine(left, y, right, y));
    }

    // nullspace lines
    std::vector<QLine> lines_null;
    lines_null.push_back(QLine(0, top, 0, bottom));
    lines_null.push_back(QLine(left, 0, right, 0));

    // draw calls
    painter->setPen(_pen_light);
    painter->drawLines(lines_light.data(), lines_light.size());

    painter->setPen(_pen_dark);
    painter->drawLines(lines_dark.data(), lines_dark.size());

    painter->setPen(_pen_null);
    painter->drawLines(lines_null.data(), lines_null.size());
}

void GraphicsNodeScene::keyPressEvent(QKeyEvent *event) {
    if (dontGrabKeyPresses) {
        QGraphicsScene::keyPressEvent(event);
        return;
    }

    switch (event->key()) {
        ////// MISC DEBUG
        case Qt::Key_Enter:
        case Qt::Key_Return:
            for (auto graphicNode : selected()) {
                cout << graphicNode->node().lock()->name() << endl;
            }
            QGraphicsScene::keyPressEvent(event);
            break;

        ////// DELETE
        case Qt::Key_X:
        case Qt::Key_Delete: {
            for (auto graphicNode : selected()) {
                remove(graphicNode);
            }
            break;
        }

        ///// DUPLICATE
        case Qt::Key_D:
            if (!(event->modifiers() == Qt::ControlModifier))
                break;  // check for Ctrl+D
        case Qt::Key_Space:
            for (auto graphicNode : selected()) {
                if (!graphicNode->node().expired()) {
                    auto copy = graphicNode->node().lock()->duplicate();
                    architecture->addNode(copy);
                    this->add(copy)->setPos(graphicNode->pos());
                }
            }
            break;

        ///// (DE-)SELECT ALL
        case Qt::Key_A: {
            bool allSelected = true;
            for (auto graphicNode : _nodes) {
                allSelected = allSelected && graphicNode->isSelected();
            }
            for (auto graphicNode : _nodes) {
                if (allSelected)
                    graphicNode->setSelected(false);
                else
                    graphicNode->setSelected(true);
            }
            break;
        }
                        
        ///// BACK TO PARENT ARCHITECTURE
        case Qt::Key_Escape: {
            if(parent_node) {
                auto topwindow = dynamic_cast<MainWindow*>(views()[0]->window());
                topwindow->set_active_scene(dynamic_cast<GraphicsNodeScene*>(parent_node->scene()));
            }
            break;
        }



        ////// NOT HANDLED -> pass forward
        default:
            QGraphicsScene::keyPressEvent(event);
    }
}
