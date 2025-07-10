#include "AppController.h"
#include "ClientSocket.h"
#include "LoginDialog.h"
#include "mainwindow.h"
#include "LoadingDialog.h"
#include <QMessageBox>
#include <QApplication>

AppController::AppController(QObject *parent) : QObject(parent) {}

AppController::~AppController()
{
    delete client;
    delete loginDialog;
    delete mainWindow;
}

void AppController::start()
{
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
    qDebug() << "Login requested";

    loginDialog->setUiEnabled(false);  // 禁用按钮，防止重复点击

    QString host = loginDialog->getHost();
    quint16 port = loginDialog->getPort();
    QString mode = loginDialog->getMode();
    QString user = loginDialog->getUsername();
    QString pass = loginDialog->getPassword();

    if (client) {
        delete client;
        client = nullptr;
    }

    // 弹出 loading 动画
    if (!loadingDialog)
    {
        loadingDialog = new LoadingDialog(loginDialog);
    }
    loadingDialog->show();

    client = new ClientSocket(this);
    connect(client, &ClientSocket::loginSuccess, this, &AppController::onLoginSuccess, Qt::QueuedConnection);
    connect(client, &ClientSocket::loginFailed, this, &AppController::onLoginFailed, Qt::QueuedConnection);
    connect(client, &ClientSocket::connectionFailed, this, &AppController::onConnectionFailed, Qt::QueuedConnection);

    client->connectToServer(host, port, [this, mode, user, pass]() {
        client->loginOrRegister(mode, user, pass);
    });

    qDebug() << "Login requested finished";
}

void AppController::onLoginSuccess()
{
    qDebug() << "Login success";
    if (loadingDialog)
    {
        loadingDialog->close();
        delete loadingDialog;
        loadingDialog = nullptr;
    }

    if (loginDialog)
    {
        loginDialog->accept();  // 此处才调用 accept()
        loginDialog = nullptr;
    }

    if (mainWindow)
    {
        mainWindow->deleteLater();
        mainWindow = nullptr;
    }

    mainWindow = new MainWindow;
    mainWindow->setClient(client);
    connect(mainWindow, &MainWindow::logoutRequested, this, &AppController::onLogoutRequested, Qt::QueuedConnection);
    mainWindow->show();
}


void AppController::onLoginFailed(const QString &reason)
{
    qDebug() << "Login failed:" << reason;
    if (loadingDialog)
    {
        loadingDialog->close();
        delete loadingDialog;
        loadingDialog = nullptr;
    }

    QMessageBox::warning(loginDialog, "登录失败", reason);
    loginDialog->setUiEnabled(true);  // 恢复可点击
}

void AppController::onConnectionFailed(const QString &reason)
{
    if (loadingDialog)
    {
        loadingDialog->close();
        delete loadingDialog;
        loadingDialog = nullptr;
    }

    qDebug() << "connect failed:" << reason;
    QMessageBox::warning(loginDialog, "连接失败", reason);
    loginDialog->setUiEnabled(true);  // 恢复可点击
}


void AppController::onLogoutRequested()
{
    if (mainWindow)
    {
        mainWindow->close();
        mainWindow->deleteLater();
        mainWindow = nullptr;
    }
    if (client)
    {
        client->deleteLater();
        client = nullptr;
    }
    QMessageBox::information(nullptr, "Logged out", "You have been logged out.");
    showLoginDialog();
}