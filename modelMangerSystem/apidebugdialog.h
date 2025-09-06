#ifndef APIDEBUGDIALOG_H
#define APIDEBUGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class ApiDebugDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ApiDebugDialog(QWidget *parent = nullptr);

private slots:
    void onSendRequest();
    void onAddParameter();
    void onRemoveParameter();
    void onParameterChanged();
    void onRequestFinished();
    void onPresetChanged();

private:
    void setupUI();
    void applyStyles();
    void sendHttpRequest();
    QString formatJson(const QString &jsonString);
    void updateSendButtonState();
    void saveSettings();
    void loadSettings();
    void setupPresetApis();

    // UI组件
    QLineEdit *urlEdit;
    QComboBox *methodCombo;
    QLineEdit *tokenEdit;
    QComboBox *presetCombo;
    QTableWidget *parameterTable;
    QPushButton *addParamButton;
    QPushButton *removeParamButton;
    QPushButton *sendButton;
    QPushButton *closeButton;
    QTextEdit *responseEdit;
    QLabel *statusLabel;

    // 网络管理
    QNetworkAccessManager *networkManager;
};

#endif // APIDEBUGDIALOG_H

