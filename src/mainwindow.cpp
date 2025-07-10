#include "mainwindow.h"
#include "ClientSocket.h"
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    chatView = new QTextEdit(this);
    chatView->setReadOnly(true);

    inputBox = new QLineEdit(this);
    sendButton = new QPushButton("Send", this);
    logoutButton = new QPushButton("Logout", this);  // 创建登出按钮

    auto *layout = new QVBoxLayout;
    layout->addWidget(chatView);
    layout->addWidget(inputBox);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(sendButton);
    btnLayout->addWidget(logoutButton); // 添加登出按钮到布局
    layout->addLayout(btnLayout);

    central->setLayout(layout);

    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
}

MainWindow::~MainWindow() {}

void MainWindow::setClient(ClientSocket *client) {
    client_ = client;
    connect(client_, &ClientSocket::messageReceived, this, &MainWindow::onMessageReceived);
}

void MainWindow::onSendClicked() {
    if (!client_) return;
    QString msg = inputBox->text().trimmed();
    if (msg.isEmpty()) return;
    // 发送消息（你需要实现 ClientSocket 的sendJson 或 sendMessage方法）
    // 例如: client_->sendMessage(msg);

    QJsonObject json;
    json["type"] = "chat";
    json["message"] = msg;

    client_->sendJson(json);
    inputBox->clear();

}

void MainWindow::onMessageReceived(const QString &msg) {
    chatView->append(msg);
}

void MainWindow::onLogoutClicked() {
    emit logoutRequested();
}
