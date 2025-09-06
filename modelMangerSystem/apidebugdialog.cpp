#include "apidebugdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QDebug>
#include <QSslSocket>
#include <QSslConfiguration>
#include <QSettings>
#include <QComboBox>

ApiDebugDialog::ApiDebugDialog(QWidget *parent)
    : QDialog(parent)
    , networkManager(new QNetworkAccessManager(this))
{
    setWindowTitle("API调试工具");
    setFixedSize(1000, 700);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    
    setupUI();
    applyStyles();
    setupPresetApis();
    loadSettings();
    
    // 连接网络请求完成信号
    connect(networkManager, &QNetworkAccessManager::finished, this, &ApiDebugDialog::onRequestFinished);
}

void ApiDebugDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // 请求配置区域
    QGroupBox *requestGroup = new QGroupBox("请求配置");
    QGridLayout *requestLayout = new QGridLayout(requestGroup);
    
    // URL输入
    requestLayout->addWidget(new QLabel("API地址:"), 0, 0);
    urlEdit = new QLineEdit();
    urlEdit->setPlaceholderText("请输入API请求地址，如: https://api.example.com/test");
    requestLayout->addWidget(urlEdit, 0, 1);
    
    // 请求方法选择
    requestLayout->addWidget(new QLabel("请求方法:"), 1, 0);
    methodCombo = new QComboBox();
    methodCombo->addItems({"GET", "POST", "PUT", "DELETE", "PATCH"});
    methodCombo->setCurrentText("GET");
    requestLayout->addWidget(methodCombo, 1, 1);
    
    // 认证令牌输入
    requestLayout->addWidget(new QLabel("认证令牌:"), 2, 0);
    tokenEdit = new QLineEdit();
    tokenEdit->setPlaceholderText("请输入Bearer令牌，如: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...");
    tokenEdit->setEchoMode(QLineEdit::Password);
    requestLayout->addWidget(tokenEdit, 2, 1);
    
    // 预设API选择
    requestLayout->addWidget(new QLabel("预设API:"), 3, 0);
    presetCombo = new QComboBox();
    presetCombo->addItem("选择预设API...");
    requestLayout->addWidget(presetCombo, 3, 1);
    
    mainLayout->addWidget(requestGroup);

    // 参数配置区域
    QGroupBox *paramGroup = new QGroupBox("请求参数");
    QVBoxLayout *paramLayout = new QVBoxLayout(paramGroup);
    
    // 参数表格
    parameterTable = new QTableWidget();
    parameterTable->setColumnCount(2);
    parameterTable->setHorizontalHeaderLabels(QStringList() << "参数名" << "参数值");
    parameterTable->horizontalHeader()->setStretchLastSection(true);
    parameterTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    parameterTable->setAlternatingRowColors(true);
    parameterTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    parameterTable->setMaximumHeight(200);
    
    // 参数操作按钮
    QHBoxLayout *paramButtonLayout = new QHBoxLayout();
    addParamButton = new QPushButton("添加参数");
    removeParamButton = new QPushButton("删除参数");
    paramButtonLayout->addWidget(addParamButton);
    paramButtonLayout->addWidget(removeParamButton);
    paramButtonLayout->addStretch();
    
    paramLayout->addWidget(parameterTable);
    paramLayout->addLayout(paramButtonLayout);
    
    mainLayout->addWidget(paramGroup);

    // 操作按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    sendButton = new QPushButton("发送请求");
    closeButton = new QPushButton("关闭");
    statusLabel = new QLabel("就绪");
    
    buttonLayout->addWidget(statusLabel);
    buttonLayout->addStretch();
    buttonLayout->addWidget(sendButton);
    buttonLayout->addWidget(closeButton);
    
    mainLayout->addLayout(buttonLayout);

    // 响应区域
    QGroupBox *responseGroup = new QGroupBox("响应结果");
    QVBoxLayout *responseLayout = new QVBoxLayout(responseGroup);
    
    responseEdit = new QTextEdit();
    responseEdit->setReadOnly(true);
    responseEdit->setFont(QFont("Consolas", 10));
    responseEdit->setPlaceholderText("响应结果将显示在这里...");
    responseLayout->addWidget(responseEdit);
    
    mainLayout->addWidget(responseGroup);

    // 连接信号
    connect(sendButton, &QPushButton::clicked, this, &ApiDebugDialog::onSendRequest);
    connect(addParamButton, &QPushButton::clicked, this, &ApiDebugDialog::onAddParameter);
    connect(removeParamButton, &QPushButton::clicked, this, &ApiDebugDialog::onRemoveParameter);
    connect(closeButton, &QPushButton::clicked, this, [this]() {
        saveSettings();
        accept();
    });
    connect(parameterTable, &QTableWidget::itemChanged, this, &ApiDebugDialog::onParameterChanged);
    connect(urlEdit, &QLineEdit::textChanged, this, &ApiDebugDialog::updateSendButtonState);
    connect(tokenEdit, &QLineEdit::textChanged, this, &ApiDebugDialog::updateSendButtonState);
    connect(presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ApiDebugDialog::onPresetChanged);
    
    // 初始化状态
    updateSendButtonState();
}

void ApiDebugDialog::applyStyles()
{
    setStyleSheet(R"(
        QDialog {
            background: #f8f9fa;
        }
        
        QGroupBox {
            font-weight: 600;
            font-size: 14px;
            color: #495057;
            border: 2px solid #dee2e6;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 10px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        
        QLineEdit {
            border: 2px solid #ced4da;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 14px;
            background: #ffffff;
        }
        
        QLineEdit:focus {
            border-color: #007bff;
            outline: none;
        }
        
        QComboBox {
            border: 2px solid #ced4da;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 14px;
            background: #ffffff;
        }
        
        QComboBox:focus {
            border-color: #007bff;
        }
        
        QTableWidget {
            border: 1px solid #dee2e6;
            border-radius: 6px;
            background: #ffffff;
            gridline-color: #dee2e6;
        }
        
        QTableWidget::item {
            padding: 8px;
            border-bottom: 1px solid #f8f9fa;
        }
        
        QTableWidget::item:selected {
            background: #e3f2fd;
            color: #1976d2;
        }
        
        QHeaderView::section {
            background: #f8f9fa;
            padding: 10px;
            border: none;
            border-bottom: 2px solid #dee2e6;
            font-weight: 600;
        }
        
        QPushButton {
            background: #007bff;
            border: none;
            border-radius: 6px;
            color: #ffffff;
            font-size: 14px;
            font-weight: 600;
            padding: 10px 20px;
        }
        
        QPushButton:hover {
            background: #0056b3;
        }
        
        QPushButton:pressed {
            background: #004085;
        }
        
        QPushButton:disabled {
            background: #6c757d;
        }
        
        QTextEdit {
            border: 1px solid #dee2e6;
            border-radius: 6px;
            padding: 10px;
            background: #ffffff;
            font-family: 'Consolas', 'Monaco', monospace;
        }
        
        QLabel {
            color: #495057;
            font-size: 14px;
        }
    )");
}

void ApiDebugDialog::onSendRequest()
{
    if (urlEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入API地址！");
        return;
    }
    
    statusLabel->setText("发送请求中...");
    sendButton->setEnabled(false);
    responseEdit->clear();
    
    sendHttpRequest();
}

void ApiDebugDialog::onAddParameter()
{
    int row = parameterTable->rowCount();
    parameterTable->insertRow(row);
    
    QTableWidgetItem *keyItem = new QTableWidgetItem();
    QTableWidgetItem *valueItem = new QTableWidgetItem();
    
    parameterTable->setItem(row, 0, keyItem);
    parameterTable->setItem(row, 1, valueItem);
    
    parameterTable->editItem(keyItem);
}

void ApiDebugDialog::onRemoveParameter()
{
    int currentRow = parameterTable->currentRow();
    if (currentRow >= 0) {
        parameterTable->removeRow(currentRow);
    }
}

void ApiDebugDialog::onParameterChanged()
{
    updateSendButtonState();
}

void ApiDebugDialog::onRequestFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    statusLabel->setText("请求完成");
    sendButton->setEnabled(true);
    
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QString responseText = QString::fromUtf8(data);
        
        // 获取HTTP状态码
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        
        // 尝试格式化JSON
        QString formattedJson = formatJson(responseText);
        responseEdit->setPlainText(formattedJson);
        
        if (statusCode >= 200 && statusCode < 300) {
            statusLabel->setText(QString("请求成功 - 状态码: %1").arg(statusCode));
        } else {
            statusLabel->setText(QString("请求完成但服务器返回错误 - 状态码: %1").arg(statusCode));
        }
    } else {
        QString errorMsg = reply->errorString();
        responseEdit->setPlainText(QString("请求失败: %1\n错误代码: %2").arg(errorMsg).arg(reply->error()));
        statusLabel->setText("请求失败");
    }
    
    reply->deleteLater();
}

void ApiDebugDialog::sendHttpRequest()
{
    QUrl url(urlEdit->text().trimmed());
    if (!url.isValid()) {
        statusLabel->setText("无效的URL");
        sendButton->setEnabled(true);
        return;
    }
    
    // 检查SSL支持
    if (url.scheme() == "https" && !QSslSocket::supportsSsl()) {
        statusLabel->setText("SSL不支持，请使用HTTP或安装SSL库");
        sendButton->setEnabled(true);
        return;
    }
    
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    
    // 添加认证令牌到请求头
    QString token = tokenEdit->text().trimmed();
    if (!token.isEmpty()) {
        // 如果令牌不包含"Bearer "前缀，自动添加
        if (!token.startsWith("Bearer ")) {
            token = "Bearer " + token;
        }
        request.setRawHeader("Authorization", token.toUtf8());
    }
    
    // 忽略SSL错误（仅用于调试）
    if (url.scheme() == "https") {
        request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    }
    
    QString method = methodCombo->currentText();
    QNetworkReply *reply = nullptr;
    
    if (method == "GET") {
        // 构建查询参数
        QUrlQuery query;
        for (int i = 0; i < parameterTable->rowCount(); ++i) {
            QTableWidgetItem *keyItem = parameterTable->item(i, 0);
            QTableWidgetItem *valueItem = parameterTable->item(i, 1);
            if (keyItem && valueItem && !keyItem->text().isEmpty()) {
                query.addQueryItem(keyItem->text(), valueItem->text());
            }
        }
        if (!query.isEmpty()) {
            url.setQuery(query);
            request.setUrl(url);
        }
        reply = networkManager->get(request);
    } else {
        // 构建POST数据
        QUrlQuery query;
        for (int i = 0; i < parameterTable->rowCount(); ++i) {
            QTableWidgetItem *keyItem = parameterTable->item(i, 0);
            QTableWidgetItem *valueItem = parameterTable->item(i, 1);
            if (keyItem && valueItem && !keyItem->text().isEmpty()) {
                query.addQueryItem(keyItem->text(), valueItem->text());
            }
        }
        
        QByteArray postData = query.toString(QUrl::FullyEncoded).toUtf8();
        reply = networkManager->post(request, postData);
    }
    
    if (reply) {
        connect(reply, &QNetworkReply::finished, this, &ApiDebugDialog::onRequestFinished);
    }
}

QString ApiDebugDialog::formatJson(const QString &jsonString)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    
    if (error.error == QJsonParseError::NoError) {
        return doc.toJson(QJsonDocument::Indented);
    } else {
        return jsonString; // 如果不是JSON，返回原始文本
    }
}

void ApiDebugDialog::updateSendButtonState()
{
    bool hasUrl = !urlEdit->text().trimmed().isEmpty();
    sendButton->setEnabled(hasUrl);
}

void ApiDebugDialog::saveSettings()
{
    QSettings settings("ApiDebugTool", "Settings");
    settings.setValue("url", urlEdit->text());
    settings.setValue("method", methodCombo->currentText());
    settings.setValue("token", tokenEdit->text());
}

void ApiDebugDialog::loadSettings()
{
    QSettings settings("ApiDebugTool", "Settings");
    urlEdit->setText(settings.value("url", "").toString());
    methodCombo->setCurrentText(settings.value("method", "GET").toString());
    tokenEdit->setText(settings.value("token", "").toString());
}

void ApiDebugDialog::setupPresetApis()
{
    presetCombo->addItem("登录 - POST /test/face-api/desktop/user/login");
    presetCombo->addItem("获取用户信息 - GET /test/face-api/desktop/user/getInfo");
    presetCombo->addItem("自定义API");
}

void ApiDebugDialog::onPresetChanged()
{
    int index = presetCombo->currentIndex();
    if (index == 0) { // 登录API
        urlEdit->setText("https://www.lihevitality.com/test/face-api/desktop/user/login");
        methodCombo->setCurrentText("POST");
        tokenEdit->clear();
        
        // 清空参数表格并添加登录参数
        parameterTable->setRowCount(0);
        onAddParameter();
        parameterTable->item(0, 0)->setText("userName");
        parameterTable->item(0, 1)->setText("it1");
        onAddParameter();
        parameterTable->item(1, 0)->setText("password");
        parameterTable->item(1, 1)->setText("123456");
        
    } else if (index == 1) { // 获取用户信息API
        urlEdit->setText("https://www.lihevitality.com/test/face-api/desktop/user/getInfo");
        methodCombo->setCurrentText("GET");
        // 不清空token，让用户手动输入或使用已保存的token
        
        // 清空参数表格（GET请求通常不需要参数）
        parameterTable->setRowCount(0);
    }
    // index == 2 是自定义API，不做任何操作
}
