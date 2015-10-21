#ifndef EDITABLELABEL_HPP
#define EDITABLELABEL_HPP

#include <QCursor>
#include <QGraphicsTextItem>
#include <QKeyEvent>

class EditableLabel : public QGraphicsTextItem {
    Q_OBJECT
   public:
    EditableLabel(QGraphicsItem *parent = 0);

    void keyPressEvent(QKeyEvent *event) override;

    void focusInEvent(QFocusEvent *event) override;

    void focusOutEvent(QFocusEvent *event) override;

signals:
    void contentUpdated(QString content);
};

class EditableDescription : public EditableLabel {
   public:
    EditableDescription(QGraphicsItem *parent = 0);

    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

   private:
    bool _is_editing;

    qreal _base_width;
    qreal _base_z;
    QFont _base_font;

    QFont _typewriter_font;
};
#endif
