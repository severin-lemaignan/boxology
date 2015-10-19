#include <QTextCursor>

#include "scene.hpp"

#include "editablelabel.hpp"

EditableLabel::EditableLabel(QGraphicsItem *parent) :
    QGraphicsTextItem(parent)
{
    this->setDefaultTextColor(Qt::white);
    this->setTextInteractionFlags(Qt::TextEditorInteraction);
    this->setCursor(Qt::IBeamCursor);
}

void EditableLabel::keyPressEvent(QKeyEvent *event) {
    if ((event->key() == Qt::Key_Enter) ||
            (event->key() == Qt::Key_Return) ||
            (event->key() == Qt::Key_Escape)) {
        clearFocus();
    } else {
        QGraphicsTextItem::keyPressEvent(event);
    }
}

void EditableLabel::focusInEvent(QFocusEvent *event) {

    dynamic_cast<GraphicsNodeScene*>(scene())->dontGrabKeyPresses = true;
    QGraphicsTextItem::focusInEvent(event);
}

void EditableLabel::focusOutEvent(QFocusEvent *event) {
    QTextCursor cur(document());
    cur.clearSelection();
    setTextCursor(cur);

    dynamic_cast<GraphicsNodeScene*>(scene())->dontGrabKeyPresses = false;
    emit contentUpdated(toPlainText());
    QGraphicsTextItem::focusOutEvent(event);
}


