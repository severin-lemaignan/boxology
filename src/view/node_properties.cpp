#include "node_properties.hpp"
#include "node.hpp"
#include <QHeaderView>

NodeProperties::NodeProperties(QWidget *parent, ConstNodePtr node)
    : QDialog(parent) {

  // Create the Description field
  descriptionEdit = new QLineEdit(this);
  repoUrlText = new QLineEdit(this);
  repoUrlText->setPlaceholderText("git@gitlab/... or https://github.com/...");
  docUrlText = new QLineEdit(this);
  docUrlText->setPlaceholderText("https://...");

  // Create the Category drop-down
  categoryComboBox = new QComboBox(this);
  categoryComboBox->addItem("Mission");
  categoryComboBox->addItem("Task");
  categoryComboBox->addItem("Skill");

  // Function domain
  domainComboBox = new QComboBox(this);
  for (const auto &entry : LABEL_NAMES) {
    domainComboBox->addItem(QString::fromStdString(entry.second));
  }

  // Create the ROS node checkbox
  rosNodeCheckbox = new QCheckBox("ROS node", this);
  connect(rosNodeCheckbox, &QCheckBox::toggled, this,
          &NodeProperties::onRosNodeCheckboxToggled);

  // Create the Done button
  doneButton = new QPushButton("Done", this);
  connect(doneButton, &QPushButton::clicked, this,
          &NodeProperties::onDoneButtonClicked);

  // Create sections
  createInputsSection();
  createServicesSection();
  createActionsSection();
  createParametersSection();

  // Layout setup
  QFormLayout *formLayout = new QFormLayout;
  formLayout->addRow("Description:", descriptionEdit);
  formLayout->addRow("Category:", categoryComboBox);
  formLayout->addRow("Functional domain:", domainComboBox);
  formLayout->addRow("Repo URL:", repoUrlText);
  formLayout->addRow("Documentation URL:", docUrlText);
  formLayout->addRow(rosNodeCheckbox);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(formLayout);
  mainLayout->addWidget(inputsGroup);
  mainLayout->addWidget(servicesGroup);
  mainLayout->addWidget(actionsGroup);
  mainLayout->addWidget(parametersGroup);
  mainLayout->addWidget(doneButton);

  setLayout(mainLayout);
  setWindowTitle(QString::fromStdString(node->name() + " properties"));

  resize(800, 600);
  // Initially disable ROS sections
  onRosNodeCheckboxToggled(false);
}

void NodeProperties::createInputsSection() {
  inputsGroup = new QGroupBox("Subscribers and servers", this);
  inputsTable = new QTableWidget(0, 4, this);
  inputsTable->setHorizontalHeaderLabels(
      {"Name", "Interface", "Datatype", "Description"});
  inputsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  inputsTable->setSortingEnabled(true);

  QPushButton *addInputButton = new QPushButton("New", this);
  connect(addInputButton, &QPushButton::clicked, this,
          &NodeProperties::addInputRow);

  QPushButton *removeInputButton = new QPushButton("Remove selected", this);
  connect(removeInputButton, &QPushButton::clicked, this,
          &NodeProperties::removeInputRow);

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(addInputButton);
  buttonLayout->addWidget(removeInputButton);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(inputsTable);
  layout->addLayout(buttonLayout);
  inputsGroup->setLayout(layout);
}

void NodeProperties::addInputRow() {
  int row = inputsTable->rowCount();
  inputsTable->insertRow(row);

  QTableWidgetItem *nameItem = new QTableWidgetItem();
  QTableWidgetItem *datatypeItem = new QTableWidgetItem();
  QTableWidgetItem *descriptionItem = new QTableWidgetItem();

  inputsTable->setItem(row, 0, nameItem);
  inputsTable->setItem(row, 2, datatypeItem);
  inputsTable->setItem(row, 3, descriptionItem);
  QComboBox *comboBox = new QComboBox();
  for (const auto &entry : INTERFACE_TYPE_NAMES) {
    comboBox->addItem(QString::fromStdString(entry.second));
  }
  inputsTable->setCellWidget(row, 1, comboBox);
}

void NodeProperties::removeInputRow() {
  int row = inputsTable->currentRow();
  if (row >= 0) {
    inputsTable->removeRow(row);
  }
}

// Implement similar methods for services, actions, and parameters
void NodeProperties::createServicesSection() {
  servicesGroup = new QGroupBox("Services", this);
  servicesTable = new QTableWidget(0, 4, this);
  servicesTable->setHorizontalHeaderLabels(
      {"Name", "Datatype", "Description", "Call"});
  servicesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  QPushButton *addServiceButton = new QPushButton("Add Service", this);
  connect(addServiceButton, &QPushButton::clicked, this,
          &NodeProperties::addServiceRow);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(servicesTable);
  layout->addWidget(addServiceButton);
  servicesGroup->setLayout(layout);
}

void NodeProperties::addServiceRow() {
  int row = servicesTable->rowCount();
  servicesTable->insertRow(row);

  QTableWidgetItem *nameItem = new QTableWidgetItem();
  QTableWidgetItem *datatypeItem = new QTableWidgetItem();
  QTableWidgetItem *descriptionItem = new QTableWidgetItem();
  QTableWidgetItem *callItem = new QTableWidgetItem();
  callItem->setCheckState(Qt::Unchecked);

  servicesTable->setItem(row, 0, nameItem);
  servicesTable->setItem(row, 1, datatypeItem);
  servicesTable->setItem(row, 2, descriptionItem);
  servicesTable->setItem(row, 3, callItem);
}

void NodeProperties::createActionsSection() {
  actionsGroup = new QGroupBox("Actions", this);
  actionsTable = new QTableWidget(0, 4, this);
  actionsTable->setHorizontalHeaderLabels(
      {"Name", "Datatype", "Description", "Trigger"});
  actionsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  QPushButton *addActionButton = new QPushButton("Add Action", this);
  connect(addActionButton, &QPushButton::clicked, this,
          &NodeProperties::addActionRow);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(actionsTable);
  layout->addWidget(addActionButton);
  actionsGroup->setLayout(layout);
}

void NodeProperties::addActionRow() {
  int row = actionsTable->rowCount();
  actionsTable->insertRow(row);

  QTableWidgetItem *nameItem = new QTableWidgetItem();
  QTableWidgetItem *datatypeItem = new QTableWidgetItem();
  QTableWidgetItem *descriptionItem = new QTableWidgetItem();
  QTableWidgetItem *triggerItem = new QTableWidgetItem();
  triggerItem->setCheckState(Qt::Unchecked);

  actionsTable->setItem(row, 0, nameItem);
  actionsTable->setItem(row, 1, datatypeItem);
  actionsTable->setItem(row, 2, descriptionItem);
  actionsTable->setItem(row, 3, triggerItem);
}

void NodeProperties::createParametersSection() {
  parametersGroup = new QGroupBox("Parameters", this);
  parametersTable = new QTableWidget(0, 3, this);
  parametersTable->setHorizontalHeaderLabels(
      {"Name", "Datatype", "Description"});
  parametersTable->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);

  QPushButton *addParameterButton = new QPushButton("Add Parameter", this);
  connect(addParameterButton, &QPushButton::clicked, this,
          &NodeProperties::addParameterRow);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(parametersTable);
  layout->addWidget(addParameterButton);
  parametersGroup->setLayout(layout);
}

void NodeProperties::addParameterRow() {
  int row = parametersTable->rowCount();
  parametersTable->insertRow(row);

  QTableWidgetItem *nameItem = new QTableWidgetItem();
  QTableWidgetItem *datatypeItem = new QTableWidgetItem();
  QTableWidgetItem *descriptionItem = new QTableWidgetItem();

  parametersTable->setItem(row, 0, nameItem);
  parametersTable->setItem(row, 1, datatypeItem);
  parametersTable->setItem(row, 2, descriptionItem);
}

void NodeProperties::onRosNodeCheckboxToggled(bool checked) {
  inputsGroup->setEnabled(checked);
  servicesGroup->setEnabled(checked);
  actionsGroup->setEnabled(checked);
  parametersGroup->setEnabled(checked);
}

void NodeProperties::onDoneButtonClicked() { accept(); }

QList<QMap<QString, QString>> NodeProperties::inputs() const {
  QList<QMap<QString, QString>> inputs;
  for (int row = 0; row < inputsTable->rowCount(); ++row) {
    QMap<QString, QString> input;
    input["name"] = inputsTable->item(row, 0)->text();
    input["datatype"] = inputsTable->item(row, 1)->text();
    input["description"] = inputsTable->item(row, 2)->text();
    input["sub"] = inputsTable->item(row, 3)->checkState() == Qt::Checked
                       ? "true"
                       : "false";
    input["pub"] = inputsTable->item(row, 4)->checkState() == Qt::Checked
                       ? "true"
                       : "false";
    inputs.append(input);
  }
  return inputs;
}

QList<QMap<QString, QString>> NodeProperties::services() const {
  QList<QMap<QString, QString>> services;
  for (int row = 0; row < servicesTable->rowCount(); ++row) {
    QMap<QString, QString> service;
    service["name"] = servicesTable->item(row, 0)->text();
    service["datatype"] = servicesTable->item(row, 1)->text();
    service["description"] = servicesTable->item(row, 2)->text();
    service["call"] = servicesTable->item(row, 3)->checkState() == Qt::Checked
                          ? "true"
                          : "false";
    services.append(service);
  }
  return services;
}

QList<QMap<QString, QString>> NodeProperties::actions() const {
  QList<QMap<QString, QString>> actions;
  for (int row = 0; row < actionsTable->rowCount(); ++row) {
    QMap<QString, QString> action;
    action["name"] = actionsTable->item(row, 0)->text();
    action["datatype"] = actionsTable->item(row, 1)->text();
    action["description"] = actionsTable->item(row, 2)->text();
    action["trigger"] = actionsTable->item(row, 3)->checkState() == Qt::Checked
                            ? "true"
                            : "false";
    actions.append(action);
  }
  return actions;
}

QList<QMap<QString, QString>> NodeProperties::parameters() const {
  QList<QMap<QString, QString>> parameters;
  for (int row = 0; row < parametersTable->rowCount(); ++row) {
    QMap<QString, QString> parameter;
    parameter["name"] = parametersTable->item(row, 0)->text();
    parameter["datatype"] = parametersTable->item(row, 1)->text();
    parameter["description"] = parametersTable->item(row, 2)->text();
    parameters.append(parameter);
  }
  return parameters;
}
