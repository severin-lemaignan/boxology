#include "node_properties.hpp"
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
  createTopicsSection();
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
  mainLayout->addWidget(topicsGroup);
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

void NodeProperties::createTopicsSection() {
  topicsGroup = new QGroupBox("Topics", this);
  topicsTable = new QTableWidget(0, 5, this);
  topicsTable->setHorizontalHeaderLabels(
      {"Name", "Datatype", "Description", "Sub", "Pub"});
  topicsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  QPushButton *addTopicButton = new QPushButton("New", this);
  connect(addTopicButton, &QPushButton::clicked, this,
          &NodeProperties::addTopicRow);

  QPushButton *removeTopicButton = new QPushButton("Remove selected", this);
  connect(removeTopicButton, &QPushButton::clicked, this,
          &NodeProperties::removeTopicRow);

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(addTopicButton);
  buttonLayout->addWidget(removeTopicButton);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(topicsTable);
  layout->addLayout(buttonLayout);
  topicsGroup->setLayout(layout);
}

void NodeProperties::addTopicRow() {
  int row = topicsTable->rowCount();
  topicsTable->insertRow(row);

  QTableWidgetItem *nameItem = new QTableWidgetItem();
  QTableWidgetItem *datatypeItem = new QTableWidgetItem();
  QTableWidgetItem *descriptionItem = new QTableWidgetItem();
  QTableWidgetItem *subItem = new QTableWidgetItem();
  subItem->setCheckState(Qt::Unchecked);
  QTableWidgetItem *pubItem = new QTableWidgetItem();
  pubItem->setCheckState(Qt::Unchecked);

  topicsTable->setItem(row, 0, nameItem);
  topicsTable->setItem(row, 1, datatypeItem);
  topicsTable->setItem(row, 2, descriptionItem);
  topicsTable->setItem(row, 3, subItem);
  topicsTable->setItem(row, 4, pubItem);
}

void NodeProperties::removeTopicRow() {
  int row = topicsTable->currentRow();
  if (row >= 0) {
    topicsTable->removeRow(row);
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
  topicsGroup->setEnabled(checked);
  servicesGroup->setEnabled(checked);
  actionsGroup->setEnabled(checked);
  parametersGroup->setEnabled(checked);
}

void NodeProperties::onDoneButtonClicked() { accept(); }

QList<QMap<QString, QString>> NodeProperties::topics() const {
  QList<QMap<QString, QString>> topics;
  for (int row = 0; row < topicsTable->rowCount(); ++row) {
    QMap<QString, QString> topic;
    topic["name"] = topicsTable->item(row, 0)->text();
    topic["datatype"] = topicsTable->item(row, 1)->text();
    topic["description"] = topicsTable->item(row, 2)->text();
    topic["sub"] = topicsTable->item(row, 3)->checkState() == Qt::Checked
                       ? "true"
                       : "false";
    topic["pub"] = topicsTable->item(row, 4)->checkState() == Qt::Checked
                       ? "true"
                       : "false";
    topics.append(topic);
  }
  return topics;
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
