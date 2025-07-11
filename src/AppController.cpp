#include "AppController.h"
#include "ClientSocket.h"
#include "LoginDialog.h"
#include <QMessageBox>
#include <QApplication>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <openssl/evp.h>
#include <iostream>
#include "ChatListWidget.h"
#include "ChatDetailWidget.h"
#include "LoadingDialog.h"

AppController::AppController(QObject *parent) : QObject(parent) {}

AppController::~AppController()
{
    delete client;
    delete loginDialog;
    delete loadingDialog;
    delete chatListWidget;
    delete chatDetailWidget;
    delete stackedWidget_;
    delete mainWidget;
}

void testOpenSSL()
{
    std::cout << "OpenSSL Version: " << OpenSSL_version(OPENSSL_VERSION) << std::endl;
}

void AppController::start()
{
    testOpenSSL();
    showLoginDialog();
    qDebug() << "AppController started and UI should be visible";
}

void AppController::showLoginDialog()
{
    if (!loginDialog)
    {
        loginDialog = new LoginDialog;
        connect(loginDialog, &LoginDialog::loginRequested, this, &AppController::onLoginRequested, Qt::QueuedConnection);
        connect(loginDialog, &LoginDialog::rejected, qApp, &QCoreApplication::quit, Qt::QueuedConnection);
    }
    loginDialog->show();
}

void AppController::onLoginRequested()
{
    loginDialog->setUiEnabled(false);

    QString host = loginDialog->getHost();
    quint16 port = loginDialog->getPort();
    QString mode = loginDialog->getMode();
    QString user = loginDialog->getUsername();
    QString pass = loginDialog->getPassword();

    if (client)
    {
        delete client;
        client = nullptr;
    }

    if (!loadingDialog)
    {
        loadingDialog = new LoadingDialog(loginDialog);
    }
    loadingDialog->show();

    client = new ClientSocket(this);
    connect(client, &ClientSocket::loginSuccess, this, &AppController::onLoginSuccess, Qt::QueuedConnection);
    connect(client, &ClientSocket::loginFailed, this, &AppController::onLoginFailed, Qt::QueuedConnection);
    connect(client, &ClientSocket::registerSuccess, this, &AppController::onRegisterSuccess, Qt::QueuedConnection);
    connect(client, &ClientSocket::registerFailed, this, &AppController::onRegisterFailed, Qt::QueuedConnection);
    connect(client, &ClientSocket::connectionFailed, this, &AppController::onConnectionFailed, Qt::QueuedConnection);
    connect(client, &ClientSocket::logoutSuccess, this, &AppController::onLoginSuccess, Qt::QueuedConnection);
    
    client->connectToServer(host, port, [this, mode, user, pass]()
                            { client->loginOrRegister(mode, user, pass); });
}

void AppController::onLoginSuccess()
{
    if (loadingDialog)
    {
        loadingDialog->close();
        delete loadingDialog;
        loadingDialog = nullptr;
    }

    if (loginDialog)
    {
        loginDialog->accept();
        loginDialog = nullptr;
    }

    if (!mainWidget)
    {
        mainWidget = new QWidget;
        auto *layout = new QVBoxLayout(mainWidget);

        chatListWidget = new ChatListWidget;
        chatDetailWidget = new ChatDetailWidget;

        chatListWidget->setClient(client);
        chatDetailWidget->setClient(client);

        stackedWidget_ = new QStackedWidget;
        stackedWidget_->addWidget(chatListWidget);
        stackedWidget_->addWidget(chatDetailWidget);
        layout->addWidget(stackedWidget_);
        mainWidget->setLayout(layout);

        connect(chatListWidget, &ChatListWidget::chatSelected, this, &AppController::onChatSelected);
        connect(chatListWidget, &ChatListWidget::newChatRequested, this, &AppController::onNewChatRequested);
        connect(chatDetailWidget, &ChatDetailWidget::backToListRequested, this, [this]()
                { stackedWidget_->setCurrentIndex(0); });
        connect(chatListWidget, &ChatListWidget::logoutRequested, this, &AppController::onLogoutRequested);
        connect(mainWidget, &QWidget::destroyed, this, [this]()
                {
            chatListWidget = nullptr;
            chatDetailWidget = nullptr;
            stackedWidget_ = nullptr;
            mainWidget = nullptr; });

        stackedWidget_->setCurrentIndex(0);
    }

    mainWidget->show();
}

void AppController::onChatSelected(const QString &chatId)
{
    if (chatDetailWidget && stackedWidget_)
    {
        stackedWidget_->setCurrentIndex(1);
        chatDetailWidget->setCurrentChat(chatId);
    }
}

void AppController::onNewChatRequested()
{
    bool ok = false;
    QString chatId = QInputDialog::getText(nullptr, "新建聊天", "请输入聊天用户名或群聊ID", QLineEdit::Normal, "", &ok);
    if (ok && !chatId.isEmpty())
    {
        if (chatListWidget)
        {
            chatListWidget->addChatSession(chatId);
            onChatSelected(chatId);
        }
    }
}

void AppController::onRegisterSuccess()
{
    onLoginSuccess();
}

void AppController::onRegisterFailed(const QString &reason)
{
    if (loadingDialog)
    {
        loadingDialog->close();
        delete loadingDialog;
        loadingDialog = nullptr;
    }

    QMessageBox::warning(loginDialog, "注册失败", reason);
    loginDialog->setUiEnabled(true);
}

void AppController::onLoginFailed(const QString &reason)
{
    if (loadingDialog)
    {
        loadingDialog->close();
        delete loadingDialog;
        loadingDialog = nullptr;
    }

    QMessageBox::warning(loginDialog, "登录失败", reason);
    loginDialog->setUiEnabled(true);
}

void AppController::onConnectionFailed(const QString &reason)
{
    if (loadingDialog)
    {
        loadingDialog->close();
        delete loadingDialog;
        loadingDialog = nullptr;
    }

    QMessageBox::warning(loginDialog, "连接失败", reason);
    loginDialog->setUiEnabled(true);
}

void AppController::onLogoutSuccess()
{
    if (mainWidget)
    {
        mainWidget->close();
        mainWidget->deleteLater();
        mainWidget = nullptr;
        chatListWidget = nullptr;
        chatDetailWidget = nullptr;
        stackedWidget_ = nullptr;
    }

    if (client)
    {

        client->deleteLater();
        client = nullptr;
    }

    QMessageBox::information(nullptr, "Logged out", "You have been logged out.");
    showLoginDialog();
}

void AppController::onLogoutRequested()
{
    client->loginOut();
}
