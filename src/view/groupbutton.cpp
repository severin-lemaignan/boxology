#include <QColorDialog>
#include <QString>

#include "groupbutton.hpp"

using namespace std;

GroupButton::GroupButton(QWidget *parent) :
    QPushButton(parent)
{
}

GroupButton::GroupButton(const string& label, const string& color, QWidget *parent): GroupButton(parent)
{
    setText(QString::fromStdString(label));
    setColor(QString::fromStdString(color));
}

void GroupButton::mousePressEvent(QMouseEvent *e)
{
    //if(e->button()==Qt::RightButton) {

    //    auto color = QColorDialog::getColor(_color, this, "Pick a color", QColorDialog::ShowAlphaChannel);
    //    setColor(color);
    //}
    if(e->button()==Qt::LeftButton) {
        emit triggered(text().toStdString());
        QPushButton::mousePressEvent(e);
    }

}


void GroupButton::setColor(const QColor& color)
{
    _color = color;
    auto icon = QPixmap(32,32);
    icon.fill(color);
    setIcon(QIcon(icon));
}

