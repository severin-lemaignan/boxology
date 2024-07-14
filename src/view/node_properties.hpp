#ifndef NODE_PROPERTIES_HPP
#define NODE_PROPERTIES_HPP

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "../label.hpp"
#include "../node.hpp"

class NodeProperties : public QDialog {
  Q_OBJECT

public:
  NodeProperties(QWidget *parent = nullptr, ConstNodePtr node = nullptr);

  std::string description() const {
    return descriptionEdit->text().toStdString();
  };
  std::string category() const {
    return categoryComboBox->currentText().toStdString();
  };
  Label functionalDomain() const {
    return get_label_by_name(domainComboBox->currentText().toStdString());
  };
  std::string repoUrl() const { return repoUrlText->text().toStdString(); };
  std::string docUrl() const { return docUrlText->text().toStdString(); };
  bool isRosNodeChecked() const { return rosNodeCheckbox->isChecked(); };

  QList<QMap<QString, QString>> inputs() const;
  QList<QMap<QString, QString>> services() const;
  QList<QMap<QString, QString>> actions() const;
  QList<QMap<QString, QString>> parameters() const;

private slots:
  void onRosNodeCheckboxToggled(bool checked);
  void onDoneButtonClicked();

private:
  QLineEdit *descriptionEdit;
  QLineEdit *repoUrlText;
  QLineEdit *docUrlText;
  QComboBox *categoryComboBox;
  QComboBox *domainComboBox;
  QCheckBox *rosNodeCheckbox;
  QPushButton *doneButton;

  QGroupBox *inputsGroup;
  QGroupBox *servicesGroup;
  QGroupBox *actionsGroup;
  QGroupBox *parametersGroup;

  QTableWidget *inputsTable;
  QTableWidget *servicesTable;
  QTableWidget *actionsTable;
  QTableWidget *parametersTable;

  void createInputsSection();
  void createServicesSection();
  void createActionsSection();
  void createParametersSection();

  void addInputRow();
  void removeInputRow();
  void addServiceRow();
  void addActionRow();
  void addParameterRow();
};

#endif // NODE_PROPERTIES_HPP
