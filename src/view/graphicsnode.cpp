/* See LICENSE file for copyright and license details. */

#include <QPushButton>
#include <QPen>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsProxyWidget>
#include <QLineEdit>
#include <QLabel>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextDocument>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsProxyWidget>
#include <QDebug>
#include <QColorDialog>
#include <QGraphicsView>

#include <algorithm>
#include <iostream>
#include <tuple>
#include <memory>

#include "tinybutton.hpp"
#include "editablelabel.hpp"

#include "edge.hpp"
#include "socket.hpp"
#include "scene.hpp"

#include "../app/mainwindow.hpp"

#include "graphicsnode.hpp"

using namespace std;

GraphicsNode::GraphicsNode(NodePtr node, QGraphicsObject *parent)
    : QGraphicsObject(parent),
      _node(node),
      _changed(false),
      _width(150),
      _height(120),
      _pen_default(QColor("#7F000000")),
      _pen_selected(QColor("#FFFF36A7"))
      // , _pen_selected(QColor("#FFFFA836"))
      // , _pen_selected(QColor("#FF00FF27"))
      ,
      _pen_sources(QColor("#FF000000")),
      _pen_sinks(QColor("#FF000000")),
      _brush_title(QColor("#E3212121")),
      _brush_background(QColor("#E31a1a1a")),
      _brush_sources(QColor("#FFFF7700")),
      _brush_sinks(QColor("#FF0077FF")),
      _effect(new QGraphicsDropShadowEffect()),
      _title_item(new EditableLabel(this)),
      _new_sink_btn(TinyButton::plus(this)),
      _new_source_btn(TinyButton::plus(this)) {
    for (auto p :
         {&_pen_default, &_pen_selected, &_pen_default, &_pen_selected}) {
        p->setWidth(0);
    }

    // setFlag(QGraphicsItem::ItemIsFocusable); // for keystrokes
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    _title_item->setPos(0, 0);
    _title_item->setTextWidth(_width - 2 * _lr_padding);
    QObject::connect(_title_item, &EditableLabel::contentUpdated, this,
                     &GraphicsNode::updateNode);

    connect(_new_sink_btn, &TinyButton::triggered, this,
            &GraphicsNode::add_sink);
    connect(_new_source_btn, &TinyButton::triggered, this,
            &GraphicsNode::add_source);

    // alignment?
    /*
       auto opts = q->document()->defaultTextOption();
       opts.setAlignment(Qt::AlignRight);
       q->document()->setDefaultTextOption(opts);
       */

    _effect->setBlurRadius(13.0);
    _effect->setColor(QColor("#99121212"));
    setGraphicsEffect(_effect);

    refreshNode();

    // qWarning() << "[G] Graphic node created";
}

void GraphicsNode::setTitle(const QString &title) {
    _title = title;
    _title_item->setPlainText(title);
}

GraphicsNode::~GraphicsNode() {
    disconnect();

    if (_central_proxy) delete _central_proxy;
    delete _title_item;
    delete _effect;

    // if (_node.expired())
    //    qWarning() << "[G] Widget deleted (node already dead)";
    // else
    //    qWarning() << "[G] Widget deleted (node "
    //               << QString::fromStdString(_node.lock()->name()) << ")";
}

QRectF GraphicsNode::boundingRect() const {
    return QRectF(-_pen_width / 2.0 - _socket_size, -_pen_width / 2.0,
                  _width + _pen_width / 2.0 + 2.0 * _socket_size,
                  _height + _pen_width / 2.0).normalized();
}

void GraphicsNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                         QWidget *) {
    const qreal edge_size = 10.0;
    const qreal title_height = 20.0;

    // path for the caption of this node
    QPainterPath path_title;
    path_title.setFillRule(Qt::WindingFill);
    path_title.addRoundedRect(QRect(0, 0, _width, title_height), edge_size,
                              edge_size);
    path_title.addRect(0, title_height - edge_size, edge_size, edge_size);
    path_title.addRect(_width - edge_size, title_height - edge_size, edge_size,
                       edge_size);
    painter->setPen(Qt::NoPen);
    painter->setBrush(_brush_title);
    painter->drawPath(path_title.simplified());

    // path for the content of this node
    QPainterPath path_content;
    path_content.setFillRule(Qt::WindingFill);
    path_content.addRoundedRect(
        QRect(0, title_height, _width, _height - title_height), edge_size,
        edge_size);
    path_content.addRect(0, title_height, edge_size, edge_size);
    path_content.addRect(_width - edge_size, title_height, edge_size,
                         edge_size);
    painter->setPen(Qt::NoPen);
    painter->setBrush(_brush_background);
    painter->drawPath(path_content.simplified());

    // path for the outline
    QPainterPath path_outline = QPainterPath();
    path_outline.addRoundedRect(QRect(0, 0, _width, _height), edge_size,
                                edge_size);
    painter->setPen(isSelected() ? _pen_selected : _pen_default);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path_outline.simplified());

// debug bounding box
#if 0
    QPen debugPen = QPen(QColor(Qt::red));
    debugPen.setWidth(0);
    auto r = boundingRect();
    painter->setPen(debugPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(r);

    painter->drawPoint(0,0);
#endif
}

void GraphicsNode::setSize(const qreal width, const qreal height) {
    setSize(QPointF(width, height));
}

void GraphicsNode::setSize(const QPointF size) {
    setSize(QSizeF(size.x(), size.y()));
}

void GraphicsNode::setSize(const QSizeF size) {
    _width = size.width();
    _height = size.height();
    _changed = true;
    prepareGeometryChange();
    updateGeometry();
}

QVariant GraphicsNode::itemChange(GraphicsItemChange change,
                                  const QVariant &value) {
    switch (change) {
        case QGraphicsItem::ItemSelectedChange: {
            if (value == true) {
                setZValue(1);
            } else {
                setZValue(0);
            }
            break;
        }
        case QGraphicsItem::ItemPositionChange:
        case QGraphicsItem::ItemPositionHasChanged:
            propagateChanges();
            updateNodePos();
            break;

        default:
            break;
    }

    return QGraphicsItem::itemChange(change, value);
}

shared_ptr<const GraphicsNodeSocket> GraphicsNode::add_socket(PortPtr port) {
    if (_node.expired()) {
        throw logic_error("We should not be accessing a dead node!");
    }

    shared_ptr<GraphicsNodeSocket> s;

    Socket socket{_node.lock(), port};

    if (port->direction == Port::Direction::IN) {
        s = make_shared<GraphicsNodeSocket>(socket, this);
        _sinks.push_back(s);
    } else {
        s = make_shared<GraphicsNodeSocket>(socket, this);
        _sources.push_back(s);
    }

    _changed = true;
    prepareGeometryChange();
    updateGeometry();
    return s;
}

shared_ptr<GraphicsNodeSocket> GraphicsNode::getPort(Port *port) {
    auto source =
        find_if(_sources.begin(), _sources.end(),
                [&port](shared_ptr<const GraphicsNodeSocket> gsocket) {
                    return gsocket->socket().port.lock().get() == port;
                });

    if (source == _sources.end()) {
        auto sink =
            find_if(_sinks.begin(), _sinks.end(),
                    [&port](shared_ptr<const GraphicsNodeSocket> gsocket) {
                        return gsocket->socket().port.lock().get() == port;
                    });

        if (sink == _sinks.end()) {
            qWarning() << "Trying to connect to port "
                       << QString::fromStdString(port->name) << " of node "
                       << _title << ", but it does not exist.";
            return nullptr;
        } else {
            return *sink;
        }
    } else {
        return *source;
    }
}

void GraphicsNode::disconnect() {
    for (auto s : _sinks) {
        s->disconnect();
    }
    for (auto s : _sources) {
        s->disconnect();
    }
}

void GraphicsNode::refreshNode() {
    if (_node.expired()) {
        throw logic_error("We should not be accessing a dead node!");
    }

    auto node = _node.lock();

    setPos({node->x(), node->y()});

    setTitle(QString::fromStdString(node->name()));

    auto color = QColor(QString::fromStdString(
        COGNITIVE_FUNCTION_COLORS.at(node->cognitive_function())));
    color.setAlpha(120);
    setColors(color);

    set<PortPtr> in_node = node->ports();
    set<PortPtr> existing;
    set<PortPtr> to_add;
    set<PortPtr> to_remove;

    for (auto s : _sinks) {
        existing.insert(s->socket().port.lock());
    }
    for (auto s : _sources) {
        existing.insert(s->socket().port.lock());
    }

    set_difference(in_node.begin(), in_node.end(), existing.begin(),
                   existing.end(), inserter(to_add, to_add.begin()));

    set_difference(existing.begin(), existing.end(), in_node.begin(),
                   in_node.end(), inserter(to_remove, to_remove.begin()));

    for (auto port = _sinks.begin(); port < _sinks.end();) {
        if (to_remove.count((*port)->socket().port.lock())) {
            (*port)->disconnect();
            scene()->removeItem((*port).get());
            port = _sinks.erase(port);
        } else {
            ++port;
        }
    }
    for (auto port = _sources.begin(); port < _sources.end();) {
        if (to_remove.count((*port)->socket().port.lock())) {
            (*port)->disconnect();
            scene()->removeItem((*port).get());
            port = _sources.erase(port);
        } else {
            ++port;
        }
    }

    for (auto port : to_add) {
        add_socket(port);
    }

    _changed = true;
    updateGeometry();
}

void GraphicsNode::updateNode(QString name) {
    if (_node.expired()) {
        throw logic_error("We should not be accessing a dead node!");
    }

    _node.lock()->name(name.toStdString());
}

void GraphicsNode::updateNodePos() {
    if (_node.expired()) {
        throw logic_error("We should not be accessing a dead node!");
    }

    auto node = _node.lock();
    node->x(x());
    node->y(y());
    node->width(width());
    node->height(height());

}

void GraphicsNode::add_sink() {
    _node.lock()->createPort(
        {"input", Port::Direction::IN, Port::Type::EXPLICIT});
}

void GraphicsNode::add_source() {
    _node.lock()->createPort(
        {"output", Port::Direction::OUT, Port::Type::EXPLICIT});
}

void GraphicsNode::updateGeometry() {
    if (!_changed) return;

    // compute if we have reached the minimum size
    updateSizeHints();
    _width = std::max(_min_width, _width);
    _height = std::max(_min_height, _height);

    // title
    _title_item->setTextWidth(_width);

    qreal ypos1 = _top_margin;
    for (size_t i = 0; i < _sinks.size(); i++) {
        auto s = _sinks[i];
        auto size = s->getSize();

        // sockets are centered around 0/0
        s->setPos(0, ypos1 + size.height() / 2.0);
        ypos1 += size.height() + _item_padding;
    }

    _new_sink_btn->setPos(0, ypos1 + _item_padding * 2);

    // sources are placed bottom/right
    qreal ypos2 = _height - _bottom_margin;
    for (size_t i = _sources.size(); i > 0; i--) {
        auto s = _sources[i - 1];
        auto size = s->getSize();

        ypos2 -= size.height();
        s->setPos(_width, ypos2 + size.height() / 2.0);
        ypos2 -= _item_padding;
    }

    _new_source_btn->setPos(_width, ypos2 - _item_padding * 2);

    // central widget
    if (_central_proxy != nullptr) {
        QRectF geom(_lr_padding, ypos1, _width - 2.0 * _lr_padding,
                    ypos2 - ypos1);
        _central_proxy->setGeometry(geom);
    }

    _changed = false;
    propagateChanges();
}

void GraphicsNode::setCentralWidget(QWidget *widget) {
    if (_central_proxy) delete _central_proxy;
    _central_proxy = new QGraphicsProxyWidget(this);
    _central_proxy->setWidget(widget);
    _changed = true;
    prepareGeometryChange();
    updateGeometry();
}

void GraphicsNode::updateSizeHints() {
    qreal min_width = 0.0;                            // _hard_min_width;
    qreal min_height = _top_margin + _bottom_margin;  // _hard_min_height;

    // sinks
    for (size_t i = 0; i < _sinks.size(); i++) {
        auto s = _sinks[i];
        auto size = s->getMinimalSize();

        min_height += size.height() + _item_padding;
        min_width = std::max(size.width(), min_width);
    }

    // central widget
    if (_central_proxy != nullptr) {
        auto wgt = _central_proxy->widget();
        if (wgt) {
            // only take the size hint if the value is valid, and if
            // the minimumSize is not set (similar to
            // QWidget/QLayout standard behavior
            auto sh = wgt->minimumSizeHint();
            auto sz = wgt->minimumSize();
            if (sh.isValid()) {
                if (sz.height() > 0)
                    min_height += sz.height();
                else
                    min_height += sh.height();

                if (sz.width() > 0)
                    min_width = std::max(qreal(sz.width()) + 2.0 * _lr_padding,
                                         min_width);
                else
                    min_width = std::max(qreal(sh.width()) + 2.0 * _lr_padding,
                                         min_width);
            } else {
                min_height += sh.height();
                min_width =
                    std::max(qreal(sh.width()) + 2.0 * _lr_padding, min_width);
            }
        }
    }

    // sources
    for (size_t i = 0; i < _sources.size(); i++) {
        auto s = _sources[i];
        auto size = s->getMinimalSize();

        min_height += size.height() + _item_padding;
        min_width = std::max(size.width(), min_width);
    }

    _min_width = std::max(min_width, _hard_min_width);
    _min_height = std::max(min_height, _hard_min_height);
}

void GraphicsNode::propagateChanges() {
    for (auto sink : _sinks) sink->notifyPositionChange();

    for (auto source : _sources) source->notifyPositionChange();
}

void GraphicsNode::setColors(const QColor &base) {
    _brush_background.setColor(base);
    _brush_title.setColor(base.lighter(110));

    // redraw the node
    update(boundingRect());
}

void GraphicsNode::hideHelpers()
{
    _new_sink_btn->hide();
    _new_source_btn->hide();

    for (auto socket : _sources) socket->hideHelpers();
    for (auto socket : _sinks) socket->hideHelpers();
}

void GraphicsNode::showHelpers()
{
    _new_sink_btn->show();
    _new_source_btn->show();

    for (auto socket : _sources) socket->showHelpers();
    for (auto socket : _sinks) socket->showHelpers();

}

void GraphicsNode::disableGraphicsEffects()
{
    _effect->setEnabled(false);
}

void GraphicsNode::enableGraphicsEffects()
{
    _effect->setEnabled(true);
}

void GraphicsNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {

    if (!_sub_structure_scene) {
        qDebug() << "Initializing a sub-architecture for node " << QString::fromStdString(_node.lock()->name());
        _sub_structure.reset(new Architecture(_node.lock()->uuid));
        _sub_structure->name = _node.lock()->name();
        _sub_structure->description = _node.lock()->name() + " is...";
        _sub_structure_scene.reset(new GraphicsNodeScene(_sub_structure.get(), this, scene()->parent()));
    }

    auto topwindow = dynamic_cast<MainWindow*>(scene()->views()[0]->window());
    topwindow->set_active_scene(_sub_structure_scene.get());

    QGraphicsItem::mouseDoubleClickEvent(event);
}
