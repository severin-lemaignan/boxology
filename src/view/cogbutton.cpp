#include "cogbutton.hpp"

#include <QColorDialog>
#include <QString>

using namespace std;

CogButton::CogButton(Label label, QWidget *parent)
    : QPushButton(parent), _label(label) {
    setText(QString::fromStdString(LABEL_NAMES.at(label)));
    setColor(QString::fromStdString(LABEL_COLORS.at(label)));
}

void CogButton::mousePressEvent(QMouseEvent *e) {
    // if(e->button()==Qt::RightButton) {

    //    auto color = QColorDialog::getColor(_color, this, "Pick a color",
    //    QColorDialog::ShowAlphaChannel);
    //    setColor(color);
    //}
    if (e->button() == Qt::LeftButton) {
        emit triggered(_label);
        QPushButton::mousePressEvent(e);
    }
}

void CogButton::setColor(const QColor &color) {
    auto icon = QPixmap(32, 32);
    icon.fill(color);
    setIcon(QIcon(icon));
}
