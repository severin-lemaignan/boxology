/* See LICENSE file for copyright and license details. */

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsScene>
#include <QFont>
#include <QFontMetrics>
#include <QList>
#include <QDrag>
#include <QMimeData>
#include <QMessageBox>

#include <QDebug>

#include <iostream>
#include <algorithm>

#include "tinybutton.hpp"
#include "edge.hpp"

#include "socket.hpp"

using namespace std;

// TODO:
// * prepareGeometryChange

#define PEN_COLOR_CIRCLE QColor("#FF000000")
#define PEN_COLOR_TEXT QColor("#FFFFFFFF")
#define BRUSH_COLOR_SINK QColor("#FF0077FF")
#define BRUSH_COLOR_SOURCE QColor("#FFFF7700")
#define TEXT_ALIGNMENT_SINK Qt::AlignLeft
#define TEXT_ALIGNMENT_SOURCE Qt::AlignRight

// const qreal height = 20.0;
// const qreal width = 30.0;

GraphicsNodeSocket::GraphicsNodeSocket(Socket socket, QGraphicsItem *parent)
    : QGraphicsItem(parent),
      _socket(socket),
      _socket_type(socket.port.lock()->direction),
      _pen_circle(PEN_COLOR_CIRCLE),
      _pen_unconnected_circle(PEN_COLOR_CIRCLE),
      _pen_text(PEN_COLOR_TEXT),
      _brush_circle((_socket_type == Port::Direction::IN) ? BRUSH_COLOR_SINK
                                                          : BRUSH_COLOR_SOURCE),
      _text(new EditableLabel()),
      _delete_button(TinyButton::forbidden(this)) {
    _pen_circle.setWidth(2);

    _pen_unconnected_circle.setWidth(0);
    //_pen_unconnected_circle.setStyle(Qt::DotLine);
    setAcceptDrops(true);

    //_text->setPos(0, 0);
    //_text->setTextWidth(_width - 2 * _lr_padding);

    _text->setPlainText(QString::fromStdString(socket.port.lock()->name));
    _text->setParentItem(this);
    connect(_text, &EditableLabel::contentUpdated, this,
            &GraphicsNodeSocket::setPortName);

    connect(_delete_button, &TinyButton::triggered, this,
            &GraphicsNodeSocket::onDeletion);
}

GraphicsNodeSocket::~GraphicsNodeSocket() {
    // qWarning() << "[G] Port deleted";
    delete _text;
    delete _delete_button;
}

Port::Direction GraphicsNodeSocket::socket_type() const { return _socket_type; }

bool GraphicsNodeSocket::is_sink() const {
    return _socket_type == Port::Direction::IN;
}

bool GraphicsNodeSocket::is_source() const {
    return _socket_type == Port::Direction::OUT;
}

QRectF GraphicsNodeSocket::boundingRect() const {
    QSizeF size = getSize();
    const qreal x = -_circle_radius - _pen_width / 2;
    const qreal y = -size.height() / 2.0 - _pen_width / 2;
    if (_socket_type == Port::Direction::IN)
        return QRectF(x, y, size.width(), size.height()).normalized();
    else
        return QRectF(-size.width() - x, y, size.width(), size.height())
            .normalized();
}

QSizeF GraphicsNodeSocket::getMinimalSize() const {
    QSizeF size;
    // const qreal text_height = static_cast<qreal>(fm.height());
    auto bb = _text->boundingRect();
    size.setWidth(std::max(_min_width, _circle_radius * 2 + _text_offset +
                                           bb.width() + _pen_width));
    size.setHeight(std::max(_min_height, bb.height() + _pen_width));
    return size;
}

QSizeF GraphicsNodeSocket::getSize() const { return getMinimalSize(); }

void GraphicsNodeSocket::onDeletion() {
    if (_edges.empty()) {
        _socket.node.lock()->remove_port(_socket.port.lock());
    } else {
        auto ok = QMessageBox::warning(nullptr, "Port deletion",
                                       "This port is already connected. Are "
                                       "you sure you want to delete it?",
                                       QMessageBox::Ok | QMessageBox::Cancel);

        if (ok == QMessageBox::Ok) {
            _socket.node.lock()->remove_port(_socket.port.lock());
        }
    }
}

void GraphicsNodeSocket::placeLabel() {
    auto bb = _text->boundingRect();

    QPointF corner(0, 0);
    QPointF delete_btn_corner(0, 0);
    if (_socket_type == Port::Direction::IN) {
        corner.setX(_circle_radius + _text_offset);
        delete_btn_corner.setX(corner.x() + bb.width() + (_text_offset * 2));
    } else {
        corner.setX(-_circle_radius - _text_offset - bb.width());
        delete_btn_corner.setX(corner.x() - (_text_offset * 2));
    }
    corner.setY(-bb.height() / 2);
    delete_btn_corner.setY(0);

    _text->setPos(corner);
    _delete_button->setPos(delete_btn_corner);
}

QPointF GraphicsNodeSocket::sceneAnchorPos() const { return mapToScene(0, 0); }

void GraphicsNodeSocket::hideHelpers() {_delete_button->hide();}
void GraphicsNodeSocket::showHelpers() {_delete_button->show();}

void GraphicsNodeSocket::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem * /*option*/,
                               QWidget * /*widget*/) {
    if (_edges.empty())
        painter->setPen(_pen_unconnected_circle);
    else
        painter->setPen(_pen_circle);
    painter->setBrush(_brush_circle);
    painter->drawEllipse(-_circle_radius, -_circle_radius, _circle_radius * 2,
                         _circle_radius * 2);

    placeLabel();

#if 0
// debug painting the bounding box
    QPen debugPen = QPen(QColor(Qt::red));
    debugPen.setWidth(0);
    auto r = boundingRect();
    painter->setPen(debugPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(r);

    painter->drawPoint(0,0);
    painter->drawLine(0,0, r.width(), 0);
#endif
}

void GraphicsNodeSocket::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent(event);
}

void GraphicsNodeSocket::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent(event);
}

bool GraphicsNodeSocket::isInSocketCircle(const QPointF &p) const {
    return p.x() >= -_circle_radius && p.x() <= _circle_radius &&
           p.y() >= -_circle_radius && p.y() <= _circle_radius;
}

void GraphicsNodeSocket::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
}

void GraphicsNodeSocket::connect_edge(shared_ptr<GraphicsDirectedEdge> edge) {
    _edges.insert(edge);
    notifyPositionChange();
    update();
}

void GraphicsNodeSocket::disconnect_edge(
    shared_ptr<GraphicsDirectedEdge> edge) {
    _edges.erase(edge);
    notifyPositionChange();
    update();
}

void GraphicsNodeSocket::disconnect() {
    auto edges = _edges;
    for (auto e : edges) {
        e->disconnect();
        // -> Edge::disconnect will call Socket::disconnect_edge in turn,
        // thus effectively removing the edge from _edges
    }

    if (!_edges.empty())
        throw logic_error("No more edges should be present at that point");
}

bool GraphicsNodeSocket::is_connected_to(
    std::shared_ptr<GraphicsDirectedEdge> edge) const {
    return _edges.count(edge) > 0;
}

void GraphicsNodeSocket::notifyPositionChange() {
    if (_edges.empty()) return;

    switch (_socket_type) {
        case Port::Direction::IN:
            for (const auto &e : _edges) e->set_stop(mapToScene(0, 0));
            break;
        case Port::Direction::OUT:
            for (const auto &e : _edges) e->set_start(mapToScene(0, 0));
            break;
    }
}
