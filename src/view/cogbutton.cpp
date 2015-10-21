#include <QColorDialog>
#include <QString>

#include "cogbutton.hpp"

using namespace std;

CogButton::CogButton(CognitiveFunction cognitive_function, QWidget *parent)
    : QPushButton(parent), _cognitive_function(cognitive_function) {
    setText(QString::fromStdString(
        COGNITIVE_FUNCTION_NAMES.at(cognitive_function)));
    setColor(QString::fromStdString(
        COGNITIVE_FUNCTION_COLORS.at(cognitive_function)));
}

void CogButton::mousePressEvent(QMouseEvent *e) {
    // if(e->button()==Qt::RightButton) {

    //    auto color = QColorDialog::getColor(_color, this, "Pick a color",
    //    QColorDialog::ShowAlphaChannel);
    //    setColor(color);
    //}
    if (e->button() == Qt::LeftButton) {
        emit triggered(_cognitive_function);
        QPushButton::mousePressEvent(e);
    }
}

void CogButton::setColor(const QColor &color) {
    auto icon = QPixmap(32, 32);
    icon.fill(color);
    setIcon(QIcon(icon));
}
