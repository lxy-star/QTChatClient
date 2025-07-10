#pragma once
#include <QObject>

class ClientSocket;
class LoginDialog;
class MainWindow;
class LoadingDialog;

class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(QObject *parent = nullptr);
    ~AppController();

    void start();

private slots:
    void onLoginRequested();
    void onLoginSuccess();
    void onLoginFailed(const QString &reason);
    void onLogoutRequested();
    void onConnectionFailed(const QString &reason);  // 新增

private:
    void showLoginDialog();

    ClientSocket *client = nullptr;
    LoginDialog *loginDialog = nullptr;
    MainWindow *mainWindow = nullptr;
    LoadingDialog *loadingDialog = nullptr;
};
