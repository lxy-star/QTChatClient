#include "ChatDetailWidget.h"
#include "ClientSocket.h"
#include <QHBoxLayout>
#include <QJsonObject>

ChatDetailWidget::ChatDetailWidget(QWidget *parent) : QWidget(parent) {
    chatView = new QTextEdit(this);
    chatView->setReadOnly(true);

    inputBox = new QLineEdit(this);
    sendButton = new QPushButton("发送", this);
    backButton = new QPushButton("返回列表", this);

    auto *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(inputBox);
    inputLayout->addWidget(sendButton);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(chatView);
    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(backButton);

    setLayout(mainLayout);

    connect(sendButton, &QPushButton::clicked, this, &ChatDetailWidget::onSendClicked);
    connect(backButton, &QPushButton::clicked, this, &ChatDetailWidget::backToListRequested);
}

void ChatDetailWidget::setClient(ClientSocket *client) {
    client_ = client;
    connect(client_, &ClientSocket::messageReceived, this, &ChatDetailWidget::onMessageReceived);
}

void ChatDetailWidget::setCurrentChat(const QString &chatId) {
    currentChatId = chatId;
    chatView->clear();
}

void ChatDetailWidget::onSendClicked() {
    if (!client_ || currentChatId.isEmpty()) return;

    QString msg = inputBox->text().trimmed();
    if (msg.isEmpty()) return;

    QJsonObject json;
    json["type"] = "chat";
    json["to"] = currentChatId;
    json["message"] = msg;

    client_->sendJson(json);
    inputBox->clear();

    chatView->append(QString("[你] %1").arg(msg));
}

void ChatDetailWidget::onMessageReceived(const QString &msg) {
    chatView->append(msg);
}
