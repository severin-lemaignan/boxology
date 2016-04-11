/* See LICENSE file for copyright and license details. */

#include <QPoint>
#include <utility>
#include <algorithm>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <iostream>
#include <QMetaProperty>
#include <QDebug>

#include "scene.hpp"
#include "graphicsnode.hpp"
#include "socket.hpp"

#include "edge.hpp"

using namespace std;

GraphicsDirectedEdge::GraphicsDirectedEdge(QPoint start, QPoint stop,
                                           qreal factor)
    : _label(new EditableLabel(this)),
      _in_scene(true),
      _is_connected(false),
      _effect(new QGraphicsDropShadowEffect()),
      _start(start),
      _stop(stop),
      _factor(factor) {
    // setFlag(QGraphicsItem::ItemIsSelectable);

    _pen.setColor(Qt::black);
    _pen.setWidth(2);
    setZValue(-1);

    _active_pen.setColor("#bccb16");
    _active_pen.setWidth(2);

    _effect->setBlurRadius(15.0);
    _effect->setColor(QColor("#99050505"));
    setGraphicsEffect(_effect);

    _label->setDefaultTextColor(QColor("#888888"));

    if (_label->boundingRect().width() < _label_max_width) {
        _label->setTextWidth(-1);
    } else {
        _label->setTextWidth(_label_max_width);
    }

    placeLabel();

    // qWarning() << "[G] Created edge";
}

GraphicsDirectedEdge::GraphicsDirectedEdge()
    : GraphicsDirectedEdge(QPoint(0, 0), QPoint(0, 0)) {}

GraphicsDirectedEdge::~GraphicsDirectedEdge() {
    delete _label;
    delete _effect;

    // if (_connection.expired())
    //    qWarning() << "[G] Edge deleted (connection already dead)";
    // else
    //    qWarning() << "[G] Edge deleted (connection "
    //            << QString::fromStdString(_connection.lock()->name) << ")";
}

void GraphicsDirectedEdge::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsPathItem::mousePressEvent(event);
}

void GraphicsDirectedEdge::set_start(QPoint p) {
    _start = p;
    update_path();
    placeLabel();
}

void GraphicsDirectedEdge::set_stop(QPoint p) {
    _stop = p;
    update_path();
    placeLabel();
}

void GraphicsDirectedEdge::disableGraphicsEffects()
{
    _effect->setEnabled(false);
}

void GraphicsDirectedEdge::enableGraphicsEffects()
{
    _effect->setEnabled(true);
}


void GraphicsDirectedEdge::connect(GraphicsNode* source_node, Port* source_port,
                                   GraphicsNode* sink_node, Port* sink_port) {
    connect_source(source_node->getPort(source_port)
                       .get());  // non-owning access to the socket
    connect_sink(sink_node->getPort(sink_port)
                     .get());  // non-owning access to the socket
}

void GraphicsDirectedEdge::establishConnection() {
    _is_connected = true;
    emit connectionEstablished(shared_from_this());

    if (_connection.use_count() == 0)
        throw std::logic_error(
            "At that point, the edge should have its underlying connection "
            "set!!");

    _label->setPlainText(QString::fromStdString(_connection.lock()->name));
    QObject::connect(_label, &EditableLabel::contentUpdated, this,
                     &GraphicsDirectedEdge::setConnectionName);
}

void GraphicsDirectedEdge::connect_sink(GraphicsNodeSocket* sink) {
    if (_sink) disconnect_sink();

    _sink = sink;
    if (_sink) _sink->connect_edge(shared_from_this());

    if (_source) establishConnection();
}

void GraphicsDirectedEdge::connect_source(GraphicsNodeSocket* source) {
    if (_source) disconnect_source();

    _source = source;
    if (_source) _source->connect_edge(shared_from_this());

    if (_sink) establishConnection();
}

void GraphicsDirectedEdge::disconnect() {
    disconnect_source();
    disconnect_sink();
}

void GraphicsDirectedEdge::disconnect_sink() {
    if (_sink && _source && _sink->is_connected_to(shared_from_this()) &&
        _source->is_connected_to(shared_from_this())) {
        _is_connected = false;
        emit connectionDisrupted(shared_from_this());
    }

    if (_sink) _sink->disconnect_edge(shared_from_this());

    if (!_source || !_source->is_connected_to(shared_from_this())) {
        if (_in_scene) {
            scene()->removeItem(this);
            _in_scene = false;
        }
    }
}

void GraphicsDirectedEdge::disconnect_source() {
    if (_sink && _source && _sink->is_connected_to(shared_from_this()) &&
        _source->is_connected_to(shared_from_this())) {
        _is_connected = false;
        emit connectionDisrupted(shared_from_this());
    }

    if (_source) _source->disconnect_edge(shared_from_this());

    if (!_sink || !_sink->is_connected_to(shared_from_this())) {
        if (_in_scene) {
            scene()->removeItem(this);
            _in_scene = false;
        }
    }
}

void GraphicsDirectedEdge::placeLabel() {
    QPointF corner{_start + (_stop - _start) / 2};
    corner.setX(corner.x() - _label->boundingRect().width() / 2);
    _label->setPos(corner);
}

void GraphicsDirectedEdge::setConnectionName(const QString& name) {
    // adapt the size of the label, if needed
    if (_label->boundingRect().width() < _label_max_width) {
        _label->setTextWidth(-1);
    } else {
        _label->setTextWidth(_label_max_width);
    }

    // update the underlying name of the connection
    _connection.lock()->name = name.toStdString();
}

void GraphicsBezierEdge::update_path() {
    QPoint c1, c2;
    QPainterPath path(_start);

    // compute anchor point offsets
    const qreal min_dist = 0.f;
    // const qreal max_dist = 250.f;
    qreal dist = 0;
    if (_start.x() <= _stop.x()) {
        dist = std::max(min_dist, (_stop.x() - _start.x()) * _factor);
    } else {
        dist = std::max(min_dist, (_start.x() - _stop.x()) * _factor);
    }

    // dist = std::min(dist, max_dist);
    c1.setX(_start.x() + dist);
    c1.setY(_start.y());

    c2.setX(_stop.x() - dist);
    c2.setY(_stop.y());

    path.cubicTo(c1, c2, _stop);
    setPath(path);
}

void GraphicsBezierEdge::paint(QPainter* painter,
                               const QStyleOptionGraphicsItem* /*option*/,
                               QWidget* /*widget*/) {
    if (_is_connected)
        painter->setPen(_pen);
    else
        painter->setPen(_active_pen);

    painter->drawPath(path());
}
