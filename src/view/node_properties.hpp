#ifndef NODE_PROPERTIES_HPP
#define NODE_PROPERTIES_HPP

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QItemDelegate>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "../label.hpp"
#include "../node.hpp"

class NonEmptyValidatingDelegate : public QItemDelegate {
  Q_OBJECT

public:
  NonEmptyValidatingDelegate(QObject *parent = nullptr)
      : QItemDelegate(parent) {}

  void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const override {
    QString value = static_cast<QLineEdit *>(editor)->text();

    if (value.isEmpty()) {
      QMessageBox::warning(nullptr, "Invalid Input",
                           "This field cannot be empty.");
      return;
    }

    QItemDelegate::setModelData(editor, model, index);
  }
};

class NodeProperties : public QDialog {
  Q_OBJECT

public:
  NodeProperties(QWidget *parent = nullptr, NodePtr node = nullptr);

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

  void fieldsFromNode(ConstNodePtr node);

  QList<QMap<QString, QString>> inputs() const;
  QList<QMap<QString, QString>> outputs() const;
  QList<QMap<QString, QString>> parameters() const;

private slots:
  void onRosNodeCheckboxToggled(bool checked);
  void onDoneButtonClicked();

private:
  NodePtr _node;

  QLineEdit *descriptionEdit;
  QLineEdit *repoUrlText;
  QLineEdit *docUrlText;
  QComboBox *categoryComboBox;
  QComboBox *domainComboBox;
  QCheckBox *rosNodeCheckbox;
  QPushButton *doneButton;

  QGroupBox *inputsGroup;
  QGroupBox *outputsGroup;
  QGroupBox *parametersGroup;

  QTableWidget *inputsTable;
  QTableWidget *outputsTable;
  QTableWidget *parametersTable;

  void createInputsSection();
  void createOutputsSection();
  void createParametersSection();

  void addInputRow();
  void removeInputRow();
  void addOutputRow();
  void removeOutputRow();
  void addParameterRow();
  void removeParameterRow();
};

#endif // NODE_PROPERTIES_HPP
