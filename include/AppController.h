#pragma once
#include <QObject>
#include <QStackedWidget>

class ClientSocket;
class LoginDialog;
class ChatListWidget;
class ChatDetailWidget;
class LoadingDialog;

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(QObject *parent = nullptr);
    ~AppController();

    void start();

private slots:
    void showLoginDialog();
    void onLoginRequested();
    void onLoginSuccess();
    void onLoginFailed(const QString &reason);
    void onRegisterSuccess();
    void onRegisterFailed(const QString &reason);
    void onConnectionFailed(const QString &reason);
    void onLogoutRequested();

    void onChatSelected(const QString &chatId);
    void onNewChatRequested();
    void onLogoutSuccess();

private:
    ClientSocket *client = nullptr;
    LoginDialog *loginDialog = nullptr;

    QWidget *mainWidget = nullptr;
    QStackedWidget *stackedWidget_ = nullptr;
    ChatListWidget *chatListWidget = nullptr;
    ChatDetailWidget *chatDetailWidget = nullptr;

    LoadingDialog *loadingDialog = nullptr;
};
