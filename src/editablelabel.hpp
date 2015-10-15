#ifndef EDITABLELABEL_HPP
#define EDITABLELABEL_HPP

#include <QGraphicsTextItem>
#include <QKeyEvent>

class EditableLabel : public QGraphicsTextItem {
    Q_OBJECT
   public:
    EditableLabel(QGraphicsItem *parent) : QGraphicsTextItem(parent) {
        this->setDefaultTextColor(Qt::white);
        this->setTextInteractionFlags(Qt::TextEditorInteraction);
        this->setCursor(Qt::IBeamCursor);
    }

    void keyPressEvent(QKeyEvent *event) override {
        if ((event->key() == Qt::Key_Enter) ||
            (event->key() == Qt::Key_Return)) {
            clearFocus();
        } else {
            QGraphicsTextItem::keyPressEvent(event);
        }
    }

    void focusOutEvent(QFocusEvent *event) override {
        emit contentUpdated(toPlainText());
        QGraphicsTextItem::focusOutEvent(event);
    }

signals:
    void contentUpdated(QString content);
};

#endif
