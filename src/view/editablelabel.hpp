#ifndef EDITABLELABEL_HPP
#define EDITABLELABEL_HPP

#include <QCursor>
#include <QFont>
#include <QGraphicsTextItem>
#include <QKeyEvent>

const static QColor TEXT_DARK_THEME = Qt::white;
const static QColor TEXT_LIGHT_THEME = QColor("#000000");

class EditableLabel : public QGraphicsTextItem {
    Q_OBJECT
   public:
    EditableLabel(QGraphicsItem *parent = 0);

    void keyPressEvent(QKeyEvent *event) override;

    void focusInEvent(QFocusEvent *event) override;

    void focusOutEvent(QFocusEvent *event) override;

    void setDarkTheme() {
        setDefaultTextColor(TEXT_DARK_THEME);
        update();
    }
    void setLightTheme() {
        setDefaultTextColor(TEXT_LIGHT_THEME);
        update();
    }

   signals:
    void contentUpdated(QString content);
};

class EditableDescription : public EditableLabel {
   public:
    EditableDescription(QGraphicsItem *parent = 0);

    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

   private:
    bool _is_editing;

    qreal _base_width;
    qreal _base_z;
    QFont _base_font;

    QFont _typewriter_font;
};
#endif
