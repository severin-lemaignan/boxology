
#ifndef __SMALLBUTTON_HPP
#define __SMALLBUTTON_HPP

#include <QGraphicsItem>
#include <QPen>
#include <QBrush>

class TinyButton : public QGraphicsObject {
    Q_OBJECT

   public:
    TinyButton(const QString& symbol = "×", QColor border = "#333",
               QColor bg = "#44111111", QColor bgHover = "#222",
               QColor color = "#44aaaaaa", QColor colorHover = "#eee",
               QGraphicsItem* parent = 0);

    static TinyButton* forbidden(QGraphicsItem* parent = 0);
    static TinyButton* plus(QGraphicsItem* parent = 0);

    virtual void paint(QPainter* painter,
                       const QStyleOptionGraphicsItem* option,
                       QWidget* widget = 0) override;

    virtual QRectF boundingRect() const override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

signals:
    void triggered();

   private:
    bool _hovered = false;
    QGraphicsSimpleTextItem* _text;
    QPen _pen;
    const qreal _pen_width = 1;
    const QBrush _brush;
    const QBrush _hover_brush;
    const QColor _text_color;
    const QColor _hover_text_color;
    const qreal _circle_radius = 6.0;
};

#endif  // SMALLBUTTON
