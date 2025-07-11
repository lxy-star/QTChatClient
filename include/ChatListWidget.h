#pragma once
#include <QWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QMap>

class ClientSocket;

class ChatListWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChatListWidget(QWidget *parent = nullptr);
    void setClient(ClientSocket *client);

    void addChatSession(const QString &chatId);

signals:
    void chatSelected(const QString &chatId);
    void newChatRequested();
    void logoutRequested();

private slots:
    void onChatItemClicked(QListWidgetItem *item);
    void onNewChatClicked();
    void onLogoutClicked();
    void onReceiveChatList(const QJsonArray &chats);
    void onVerifyResult(const QString &chatId, bool exists);

private:
    QListWidget *chatList_;
    QPushButton *newChatBtn_;
    QPushButton *logoutBtn_;
    ClientSocket *client_ = nullptr;

    QMap<QString, QListWidgetItem*> chatItems_;
};
