#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class ClientSocket;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setClient(ClientSocket *client);

signals:
    void logoutRequested();

private slots:
    void onSendClicked();
    void onMessageReceived(const QString &msg);
    void onLogoutClicked();

private:
    QTextEdit *chatView;
    QLineEdit *inputBox;
    QPushButton *sendButton;
    QPushButton *logoutButton;  // 新增登出按钮
    ClientSocket *client_ = nullptr;
};
