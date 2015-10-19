#ifndef EDITABLELABEL_HPP
#define EDITABLELABEL_HPP

#include <QCursor>
#include <QGraphicsTextItem>
#include <QKeyEvent>

class EditableLabel : public QGraphicsTextItem {
    Q_OBJECT
   public:
    EditableLabel(QGraphicsItem *parent);

    void keyPressEvent(QKeyEvent *event) override;

    void focusInEvent(QFocusEvent *event) override;

    void focusOutEvent(QFocusEvent *event) override;

signals:
    void contentUpdated(QString content);

};

#endif
