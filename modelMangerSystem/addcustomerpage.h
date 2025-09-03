#ifndef ADDCUSTOMERPAGE_H
#define ADDCUSTOMERPAGE_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QComboBox;
class QTextEdit;
class QPushButton;
class QVBoxLayout;
class QGridLayout;
class QHBoxLayout;

class AddCustomerPage : public QWidget
{
    Q_OBJECT
public:
    explicit AddCustomerPage(QWidget *parent = nullptr);

signals:
    void saveRequested();
    void startPhotoRequested();
    void cancelRequested();

private slots:
    void onSaveClicked();
    void onStartPhotoClicked();
    void onCancelClicked();

private:
    void setupUI();
    void applyStyles();
    QWidget* createInputGroup(const QString &label, const QString &placeholder, QLineEdit **edit, const QString &type);
    QWidget* createSelectGroup(const QString &label, const QString &placeholder, QComboBox **combo, const QStringList &options);

private:
    QLineEdit *nameEdit;
    QComboBox *genderCombo;
    QLineEdit *ageEdit;
    QLineEdit *phoneEdit;
    QComboBox *skinTypeCombo;
    QTextEdit *remarkEdit;
    QPushButton *saveBtn;
    QPushButton *startPhotoBtn;
    QPushButton *cancelBtn;
};

#endif // ADDCUSTOMERPAGE_H
