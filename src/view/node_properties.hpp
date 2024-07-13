#ifndef NODE_PROPERTIES_HPP
#define NODE_PROPERTIES_HPP

#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "../node.hpp"

class NodeProperties : public QDialog {
  Q_OBJECT

public:
  NodeProperties(QWidget *parent = nullptr, ConstNodePtr node = nullptr);
  QString getComboBoxValue() const;
  QString getLineEditValue() const;

private slots:
  void onDoneButtonClicked();

private:
  QComboBox *comboBox;
  QLineEdit *lineEdit;
  QPushButton *doneButton;

  QString comboBoxValue;
  QString lineEditValue;
};

#endif // NODE_PROPERTIES_HPP
