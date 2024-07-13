#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include <QDebug>

#include "tinybutton.hpp"

TinyButton::TinyButton(const QString &symbol, QColor border, QColor bg,
                       QColor bgHover, QColor color, QColor colorHover,
                       QGraphicsItem *parent)
    : QGraphicsObject(parent), _hovered(false),
      _text(new QGraphicsSimpleTextItem(symbol, this)), _pen(border),
      _brush(bg), _hover_brush(bgHover), _text_color(color),
      _hover_text_color(colorHover) {
  _text->setPos(-_text->boundingRect().width() / 2,
                -_text->boundingRect().height() / 2 - 1);
  _text->setBrush(_text_color);

  _pen.setWidth(_pen_width);

  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::LeftButton);
}

TinyButton *TinyButton::forbidden(QGraphicsItem *parent) {
  return new TinyButton("×", "#00ffffff", "#44111111", "#44111111", "#44aaaaaa",
                        "#522", parent);
}

TinyButton *TinyButton::plus(QGraphicsItem *parent) {
  return new TinyButton("+", "#7f000000", "#333", "#333", "#888", "#aaa",
                        parent);
}

TinyButton *TinyButton::plain(QColor color, QGraphicsItem *parent) {
  return new TinyButton("", "#7f000000", color, "#333", "#888", "#aaa", parent);
}

void TinyButton::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) {
  painter->setPen(_pen);

  if (_hovered) {
    painter->setBrush(_hover_brush);
    _text->setBrush(_hover_text_color);
  } else {
    painter->setBrush(_brush);
    _text->setBrush(_text_color);
  }
  painter->drawEllipse(-_circle_radius, -_circle_radius, _circle_radius * 2,
                       _circle_radius * 2);
}

QRectF TinyButton::boundingRect() const {
  return QRectF(
      -_circle_radius - _pen_width / 2, -_circle_radius - _pen_width / 2,
      _circle_radius * 2 + _pen_width, _circle_radius * 2 + _pen_width);
}

void TinyButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  _hovered = true;
  setCursor(Qt::PointingHandCursor);
  update();
}

void TinyButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  _hovered = false;
  setCursor(Qt::ArrowCursor);
  update();
}

void TinyButton::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  emit triggered();
  QGraphicsItem::mousePressEvent(event);
}
