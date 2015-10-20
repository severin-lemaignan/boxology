#include <QColorDialog>
#include <QString>

#include "groupbutton.hpp"

using namespace std;

GroupButton::GroupButton(Group group, QWidget *parent): 
    QPushButton(parent),
    _group(group)
{
    setText(QString::fromStdString(GROUPNAME.at(group)));
    setColor(QString::fromStdString(GROUPCOLORS.at(group)));
}

void GroupButton::mousePressEvent(QMouseEvent *e)
{
    //if(e->button()==Qt::RightButton) {

    //    auto color = QColorDialog::getColor(_color, this, "Pick a color", QColorDialog::ShowAlphaChannel);
    //    setColor(color);
    //}
    if(e->button()==Qt::LeftButton) {
        emit triggered(_group);
        QPushButton::mousePressEvent(e);
    }

}


void GroupButton::setColor(const QColor& color)
{
    auto icon = QPixmap(32,32);
    icon.fill(color);
    setIcon(QIcon(icon));
}

