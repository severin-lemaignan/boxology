#include "node_properties.hpp"

NodeProperties::NodeProperties(QWidget *parent, ConstNodePtr node)
    : QDialog(parent) {

  setWindowTitle(QString::fromStdString(node->name()));

  // Create the drop-down (combo box) input
  comboBox = new QComboBox(this);
  comboBox->addItem("Option 1");
  comboBox->addItem("Option 2");
  comboBox->addItem("Option 3");

  // Create the text input (line edit)
  lineEdit = new QLineEdit(this);

  // Create the "Done" button
  doneButton = new QPushButton("Done", this);
  connect(doneButton, &QPushButton::clicked, this,
          &NodeProperties::onDoneButtonClicked);

  // Arrange the widgets in a vertical layout
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(comboBox);
  layout->addWidget(lineEdit);
  layout->addWidget(doneButton);
  setLayout(layout);
}

QString NodeProperties::getComboBoxValue() const { return comboBoxValue; }

QString NodeProperties::getLineEditValue() const { return lineEditValue; }

void NodeProperties::onDoneButtonClicked() {
  // Store the values from the inputs
  comboBoxValue = comboBox->currentText();
  lineEditValue = lineEdit->text();
  accept(); // Close the dialog
}
