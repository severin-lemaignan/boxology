/* See LICENSE file for copyright and license details. */

#include <cmath>
#include <QColor>
#include <QPainter>
#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QDebug>
#include <algorithm>
#include <iostream>

#include "scene.hpp"
#include "socket.hpp"

using namespace std;

// TODO: move to graphicsnodeview. use graphicsnodescene for management

GraphicsNodeScene::GraphicsNodeScene(Architecture *architecture,
                                     QObject *parent)
    : QGraphicsScene(parent),
      _color_background(QColor("#393939")),
      _color_light(QColor("#2F2F2F")),
      _color_dark(QColor("#292929")),
      _color_null(QColor("#212121")),
      _pen_light(QPen(_color_light)),
      _pen_dark(QPen(_color_dark)),
      _pen_null(QPen(_color_null)),
      _brush_background(_color_background),
      architecture(architecture) {
    // initialize default pen settings
    for (auto p : {&_pen_light, &_pen_dark, &_pen_null}) {
        p->setWidth(0);
    }

    // initialize the background
    setBackgroundBrush(_brush_background);

    // add some text to where zero is
    auto nulltext = this->addText("(0,0)", QFont("Ubuntu Mono"));
    nulltext->setPos(0, 0);
    nulltext->setDefaultTextColor(_color_null);
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

shared_ptr<GraphicsDirectedEdge> GraphicsNodeScene::add(
    ConnectionPtr connection) {
    auto edge = make_shared<GraphicsBezierEdge>();

    // look for the *graphic nodes* that represent the source/sink of
    // our connection:
    auto source =
        find_if(_nodes.begin(), _nodes.end(),
                [&connection](const shared_ptr<GraphicsNode> gNode) {
                    return gNode->node().lock() == connection->from.node.lock();
                });

    auto sink =
        find_if(_nodes.begin(), _nodes.end(),
                [&connection](const shared_ptr<GraphicsNode> gNode) {
                    return gNode->node().lock() == connection->to.node.lock();
                });

    edge->connect(*source, connection->from.port, *sink, connection->to.port);

    connect(edge.get(), &GraphicsDirectedEdge::connectionEstablished, this,
            &GraphicsNodeScene::onConnectionEstablished);
    connect(edge.get(), &GraphicsDirectedEdge::connectionDisrupted, this,
            &GraphicsNodeScene::onConnectionDisrupted);

    _edges.insert(edge);
    addItem(edge.get());
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


void GraphicsNodeScene::onConnectionEstablished(GraphicsDirectedEdge *edge) {
    architecture->createConnection(edge->source()->socket(),
                                   edge->sink()->socket());
}

void GraphicsNodeScene::onConnectionDisrupted(GraphicsDirectedEdge *edge) {
    qWarning() << "Connection disrupted!";
    architecture->removeConnection(edge->source()->socket(),
                                   edge->sink()->socket());
}

set<shared_ptr<GraphicsNode>> GraphicsNodeScene::selected() {
    set<shared_ptr<GraphicsNode>> selectedNodes;

    for (auto graphicNode : _nodes) {
        if (graphicNode->isSelected()) {
            selectedNodes.insert(graphicNode);
        }
    }
    return selectedNodes;
}

/*
 * TODO: move the visualization into the graphicsview, and move all the GUI
 * logic into the graphicsnodescene
 */
void GraphicsNodeScene::drawBackground(QPainter *painter, const QRectF &rect) {
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
    switch (event->key()) {
        ////// MISC DEBUG
        case Qt::Key_Enter:
        case Qt::Key_Return:
            for (auto graphicNode : selected()) {
                cout << graphicNode->node().lock()->name() << endl;
            }
            break;

        ////// DELETE
        case Qt::Key_X:
        case Qt::Key_Delete: {
            for (auto graphicNode : selected()) {
                auto node = graphicNode->node();
                if (node.expired()) {
                    qWarning("Attempting to delete an already deleted node!");
                    break;
                }
                graphicNode.get()->setSelected(false);
                auto disconnected_edges = graphicNode.get()->disconnect();

                architecture->removeNode(node.lock());
                _nodes.erase(graphicNode);

                for(auto e : disconnected_edges) {
                    _edges.erase(e);
                }
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

        ////// NOT HANDLED
        default:
            QGraphicsScene::keyPressEvent(event);
    }
}
