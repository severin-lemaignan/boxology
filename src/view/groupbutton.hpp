#ifndef GROUPBUTTON_HPP
#define GROUPBUTTON_HPP

#include <string>

#include <QPushButton>
#include <QMouseEvent>
#include <QColor>

#include "../group.hpp"

class GroupButton : public QPushButton
{
    Q_OBJECT

public:
    GroupButton() = delete;
    GroupButton(Group group, QWidget *parent = 0);

    //QColor color() const {return _color;}

//private slots:
    void mousePressEvent(QMouseEvent *e);

signals:
    void triggered(Group group);

private:
    void setColor(const QColor& color);

    Group _group;
};

#endif // GROUPBUTTON_HPP
