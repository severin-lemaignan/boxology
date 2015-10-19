#ifndef GROUPBUTTON_HPP
#define GROUPBUTTON_HPP

#include <string>

#include <QPushButton>
#include <QMouseEvent>
#include <QColor>

class GroupButton : public QPushButton
{
    Q_OBJECT

public:
    explicit GroupButton(QWidget *parent = 0);
    GroupButton(const std::string& label, const std::string& color, QWidget *parent = 0);

    void setColor(const QColor& color);
    QColor color() const {return _color;}

//private slots:
    void mousePressEvent(QMouseEvent *e);

signals:
    void triggered(const std::string& group);

private:
    QColor _color;
};

#endif // GROUPBUTTON_HPP
