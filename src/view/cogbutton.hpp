#ifndef COGNITIVE_FUNCTION_BUTTON_HPP
#define COGNITIVE_FUNCTION_BUTTON_HPP

#include <string>

#include <QPushButton>
#include <QMouseEvent>
#include <QColor>

#include "../cognitive_function.hpp"

class CogButton : public QPushButton {
    Q_OBJECT

   public:
    CogButton() = delete;
    CogButton(CognitiveFunction cognitive_function, QWidget *parent = 0);

    // QColor color() const {return _color;}

    // private slots:
    void mousePressEvent(QMouseEvent *e);

signals:
    void triggered(CognitiveFunction cognitive_function);

   private:
    void setColor(const QColor &color);

    CognitiveFunction _cognitive_function;
};

#endif  // COGNITIVE_FUNCTION_BUTTON_HPP
