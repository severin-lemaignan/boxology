#ifndef LABEL_BUTTON_HPP
#define LABEL_BUTTON_HPP

#include <string>

#include <QPushButton>
#include <QMouseEvent>
#include <QColor>

#include "../label.hpp"

class CogButton : public QPushButton {
    Q_OBJECT

   public:
    CogButton() = delete;
    CogButton(Label label, QWidget *parent = 0);

    // QColor color() const {return _color;}

    // private slots:
    void mousePressEvent(QMouseEvent *e);

signals:
    void triggered(Label label);

   private:
    void setColor(const QColor &color);

    Label _label;
};

#endif  // LABEL_BUTTON_HPP
