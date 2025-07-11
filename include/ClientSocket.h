#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>   // 必须包含
#include <asio.hpp>
#include <memory>
#include <functional>

class ClientSocket : public QObject
{
    Q_OBJECT
public:
    explicit ClientSocket(QObject *parent = nullptr);
    ~ClientSocket();

    void connectToServer(const QString &host, quint16 port, std::function<void()> onConnected = nullptr);
    void loginOrRegister(const QString &mode, const QString &username, const QString &password);
    bool isConnected() const;
    void sendJson(const QJsonObject &obj);

    void requestChatList();
    void verifyTargetExistence(const QString &type, const QString &target);
    void loginOut();

signals:
    void loginSuccess();
    void registerSuccess();
    void registerFailed(const QString &reason);
    void loginFailed(const QString &reason);
    void connectionFailed(const QString &reason);
    void messageReceived(const QString &msg);
    void chatListReceived(const QJsonArray &chatList);   // 这里必须是完整类型
    void verifyUserResult(const QString &chatId, bool exists);
    void logoutSuccess();

private:
    void doRead();
    void handleJsonObject(const QJsonObject &obj);
    QString username_;

    asio::io_context io_context;
    asio::ip::tcp::socket socket_;
    std::thread io_thread;
    std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> work_guard_;

    std::shared_ptr<std::string> recvBuffer;
};
