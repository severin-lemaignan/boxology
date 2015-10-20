#include <QTextCursor>
#include <QPainter>
#include <QFontDatabase>

#include "scene.hpp"

#include "editablelabel.hpp"

EditableLabel::EditableLabel(QGraphicsItem *parent) :
    QGraphicsTextItem(parent)
{
    this->setDefaultTextColor(Qt::white);
    this->setTextInteractionFlags(Qt::TextEditorInteraction | Qt::LinksAccessibleByMouse);
    this->setCursor(Qt::IBeamCursor);
    this->setOpenExternalLinks(true);

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


EditableDescription::EditableDescription(QGraphicsItem *parent) :
    EditableLabel(parent),
    _is_editing(false),
    _typewriter_font(QFontDatabase::systemFont(QFontDatabase::FixedFont))
{
    _typewriter_font.setPixelSize(12);
}


void EditableDescription::focusInEvent(QFocusEvent *event) {

    _is_editing = true;

    _base_width = textWidth();
    _base_z = zValue();
    _base_font = font();

    auto content = toHtml();

    setTextWidth(600);
    setZValue(1000);
    setFont(_typewriter_font);

    setPlainText(content);

    EditableLabel::focusInEvent(event);
}

void EditableDescription::focusOutEvent(QFocusEvent *event) {

    _is_editing = false;

    auto content = toPlainText();

    setTextWidth(_base_width);
    setZValue(_base_z);
    setFont(_base_font);

    setHtml(content);
    EditableLabel::focusOutEvent(event);
}


void EditableDescription::paint(QPainter * painter, const QStyleOptionGraphicsItem * opt, QWidget * w)
{
    if(_is_editing) {
        QBrush brush("#ccbbbbbb");
        painter->setBrush(brush);
        painter->drawRect(boundingRect());
    }
    EditableLabel::paint(painter, opt, w);
}

