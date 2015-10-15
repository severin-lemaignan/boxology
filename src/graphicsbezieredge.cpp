/* See LICENSE file for copyright and license details. */

#include "graphicsbezieredge.hpp"
#include <QPoint>
#include <utility>
#include <algorithm>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <iostream>
#include <QMetaProperty>
#include <QDebug>


#include "graphicsnodescene.hpp"
#include "graphicsnode.hpp"
#include "graphicsnodesocket.hpp"

using namespace std;

GraphicsDirectedEdge::GraphicsDirectedEdge(QPoint start, QPoint stop, 
                                           qreal factor)
    :_pen(QColor("#00FF00")),
     _effect(new QGraphicsDropShadowEffect()),
     _start(start),
     _stop(stop),
     _factor(factor)
{
    _pen.setWidth(2);
    setZValue(-1);

    _effect->setBlurRadius(15.0);
    _effect->setColor(QColor("#99050505"));
    setGraphicsEffect(_effect);
}

GraphicsDirectedEdge::GraphicsDirectedEdge() : GraphicsDirectedEdge(QPoint(0, 0), QPoint(0, 0))
{
}


GraphicsDirectedEdge::~GraphicsDirectedEdge()
{
    delete _effect;

    if(_connection.expired())
        qWarning() << "Edge deleted (connection already dead)";
    else
        qWarning() << "Edge deleted (connection " << QString::fromStdString(_connection.lock()->name) << ")";
}


void GraphicsDirectedEdge::
mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsPathItem::mousePressEvent(event);
}

void GraphicsDirectedEdge::
set_start(QPoint p)
{
    _start = p;
    this->update_path();
}


void GraphicsDirectedEdge::
set_stop(QPoint p)
{
    _stop = p;
    update_path();
}

void GraphicsDirectedEdge::connect(shared_ptr<GraphicsNode> n1, PortWeakPtr source, 
                                   shared_ptr<GraphicsNode> n2, PortWeakPtr sink)
{

    connect_source(n1->connect_source(source.lock(), this));
    connect_sink(n2->connect_sink(sink.lock(), this));

}

void GraphicsDirectedEdge::disconnect()
{
    disconnect_source();
    disconnect_sink();
}


void GraphicsDirectedEdge::disconnect_sink()
{
    if (_sink && _sink->get_edge() && _source && _source->get_edge())
        emit connectionDisrupted(this);

    if (_sink) _sink->set_edge(nullptr);
}

void GraphicsDirectedEdge::disconnect_source()
{
    if (_sink && _sink->get_edge() && _source && _source->get_edge())
        emit connectionDisrupted(this);

    if (_source) _source->set_edge(nullptr);

}


void GraphicsDirectedEdge::connect_sink(GraphicsNodeSocket *sink)
{
    if (_sink == sink) return;


    if (_sink) disconnect_sink();

    _sink = sink;
    if (_sink) _sink->set_edge(this);

    if (_source) emit connectionEstablished(this);
}

void GraphicsDirectedEdge::connect_source(GraphicsNodeSocket *source)
{
    if (_source == source) return;

    if (_source) disconnect_source();

    _source = source;
    if (_source) _source->set_edge(this);

    if (_sink) emit connectionEstablished(this);
}

void GraphicsBezierEdge::update_path()
{
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

void GraphicsBezierEdge::paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    painter->setPen(_pen);
    painter->drawPath(path());
}



