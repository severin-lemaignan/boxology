#include "node_properties.hpp"
#include "node.hpp"
#include <QHeaderView>

NodeProperties::NodeProperties(QWidget *parent, NodePtr node)
    : QDialog(parent), _node(node) {

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
  createOutputsSection();
  createParametersSection();

  fieldsFromNode(node);

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
  mainLayout->addWidget(outputsGroup);
  mainLayout->addWidget(parametersGroup);
  mainLayout->addWidget(doneButton);

  setLayout(mainLayout);
  setWindowTitle(QString::fromStdString(node->name() + " properties"));

  resize(800, 600);

  // Initially disable ROS sections
  onRosNodeCheckboxToggled(false);
}

void NodeProperties::fieldsFromNode(ConstNodePtr node) {

  for (auto port : node->ports()) {
    auto table =
        (port->direction == Port::Direction::IN ? inputsTable : outputsTable);

    int row = table->rowCount();
    table->insertRow(row);

    QTableWidgetItem *nameItem =
        new QTableWidgetItem(QString::fromStdString(port->name));
    QTableWidgetItem *datatypeItem =
        new QTableWidgetItem(QString::fromStdString(port->datatype));
    QTableWidgetItem *descriptionItem =
        new QTableWidgetItem(QString::fromStdString(port->description));

    QComboBox *comboBox = new QComboBox();
    size_t idx = 0;
    for (const auto &entry : INTERFACE_TYPE_NAMES) {
      comboBox->addItem(QString::fromStdString(entry.second));
      if (port->type == entry.first) {
        comboBox->setCurrentIndex(idx);
      }
      idx += 1;
    }

    table->setItem(row, 0, nameItem);
    table->setCellWidget(row, 1, comboBox);
    table->setItem(row, 2, datatypeItem);
    table->setItem(row, 3, descriptionItem);
  }
}

void NodeProperties::createInputsSection() {
  inputsGroup = new QGroupBox("Inputs: Subscribers and servers", this);
  inputsTable = new QTableWidget(0, 4, this);
  inputsTable->setHorizontalHeaderLabels(
      {"Name", "Interface", "Datatype", "Description"});
  inputsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  inputsTable->setSortingEnabled(true);

  // ensure enought vertical space for 5 rows + header
  inputsTable->setMinimumHeight(inputsTable->horizontalHeader()->height() * 6);

  inputsTable->setItemDelegateForColumn(0,
                                        new NonEmptyValidatingDelegate(this));

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

void NodeProperties::createOutputsSection() {
  outputsGroup = new QGroupBox("Outputs: Publishers and callers", this);
  outputsTable = new QTableWidget(0, 4, this);
  outputsTable->setHorizontalHeaderLabels(
      {"Name", "Interface", "Datatype", "Description"});
  outputsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  outputsTable->setSortingEnabled(true);

  // ensure enought vertical space for 5 rows + header
  outputsTable->setMinimumHeight(outputsTable->horizontalHeader()->height() *
                                 6);

  outputsTable->setItemDelegateForColumn(0,
                                         new NonEmptyValidatingDelegate(this));

  QPushButton *addOutputButton = new QPushButton("New", this);
  connect(addOutputButton, &QPushButton::clicked, this,
          &NodeProperties::addOutputRow);

  QPushButton *removeOutputButton = new QPushButton("Remove selected", this);
  connect(removeOutputButton, &QPushButton::clicked, this,
          &NodeProperties::removeOutputRow);

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(addOutputButton);
  buttonLayout->addWidget(removeOutputButton);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(outputsTable);
  layout->addLayout(buttonLayout);
  outputsGroup->setLayout(layout);
}

void NodeProperties::addOutputRow() {
  int row = outputsTable->rowCount();
  outputsTable->insertRow(row);

  QTableWidgetItem *nameItem = new QTableWidgetItem();
  QTableWidgetItem *datatypeItem = new QTableWidgetItem();
  QTableWidgetItem *descriptionItem = new QTableWidgetItem();

  outputsTable->setItem(row, 0, nameItem);
  outputsTable->setItem(row, 2, datatypeItem);
  outputsTable->setItem(row, 3, descriptionItem);
  QComboBox *comboBox = new QComboBox();
  for (const auto &entry : INTERFACE_TYPE_NAMES) {
    comboBox->addItem(QString::fromStdString(entry.second));
  }
  outputsTable->setCellWidget(row, 1, comboBox);
}

void NodeProperties::removeOutputRow() {
  int row = outputsTable->currentRow();
  if (row >= 0) {
    outputsTable->removeRow(row);
  }
}

void NodeProperties::createParametersSection() {
  parametersGroup = new QGroupBox("Parameters", this);
  parametersTable = new QTableWidget(0, 3, this);
  parametersTable->setHorizontalHeaderLabels(
      {"Name", "Datatype", "Description"});
  parametersTable->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);

  parametersTable->setItemDelegateForColumn(
      0, new NonEmptyValidatingDelegate(this));

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

void NodeProperties::removeParameterRow() {
  int row = parametersTable->currentRow();
  if (row >= 0) {
    parametersTable->removeRow(row);
  }
}

void NodeProperties::onRosNodeCheckboxToggled(bool checked) {
  //  inputsGroup->setEnabled(checked);
  //  outputsGroup->setEnabled(checked);
  //  parametersGroup->setEnabled(checked);
}

void NodeProperties::onDoneButtonClicked() {

  _node->description(descriptionEdit->text().toStdString());
  _node->type(
      get_node_type_by_name(categoryComboBox->currentText().toStdString()));
  _node->label(get_label_by_name(domainComboBox->currentText().toStdString()));
  _node->repoUrl(repoUrlText->text().toStdString());
  _node->docUrl(docUrlText->text().toStdString());

  std::set<std::string> port_names;

  for (auto const &table : {inputsTable, outputsTable}) {
    for (int row = 0; row < table->rowCount(); ++row) {

      auto name = table->item(row, 0)->text().toStdString();
      port_names.insert(name);

      auto datatype = table->item(row, 1)->text().toStdString();
      auto description = table->item(row, 2)->text().toStdString();

      auto direction =
          table == inputsTable ? Port::Direction::IN : Port::Direction::OUT;

      auto port = _node->port(name);
      if (!port) {
        port = _node->createPort({name, direction, InterfaceType::UNKNOWN});
      }

      port->direction = direction;
      port->datatype = datatype;
      port->description = description;
      port->type = get_interface_type_by_name(table->cellWidget(row, 1)
                                                  ->property("currentText")
                                                  .toString()
                                                  .toStdString());
    }
  }

  // finally, remove the ports that have been removed from the list
  std::set<PortPtr> ports = _node->ports();
  for (auto &p : ports) {
    if (port_names.count(p->name) == 0) {
      _node->removePort(p);
    }
  }

  accept();
}

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

QList<QMap<QString, QString>> NodeProperties::outputs() const {
  QList<QMap<QString, QString>> outputs;
  for (int row = 0; row < outputsTable->rowCount(); ++row) {
    QMap<QString, QString> output;
    output["name"] = outputsTable->item(row, 0)->text();
    output["datatype"] = outputsTable->item(row, 1)->text();
    output["description"] = outputsTable->item(row, 2)->text();
    output["sub"] = outputsTable->item(row, 3)->checkState() == Qt::Checked
                        ? "true"
                        : "false";
    output["pub"] = outputsTable->item(row, 4)->checkState() == Qt::Checked
                        ? "true"
                        : "false";
    outputs.append(output);
  }
  return outputs;
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
