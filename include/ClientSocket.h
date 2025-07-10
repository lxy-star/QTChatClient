#pragma once
#include <asio.hpp>
#include <asio/steady_timer.hpp> // 避免某些平台警告
#include <QString>
#include <QObject>
#include <QJsonObject>
#include <QThread>
#include <memory>

class ClientSocket : public QObject
{
    Q_OBJECT
public:
    explicit ClientSocket(QObject *parent = nullptr);
    ~ClientSocket();

    // 新增回调参数onConnected
    void connectToServer(const QString &host, quint16 port, std::function<void()> onConnected = nullptr);
    void loginOrRegister(const QString &mode, const QString &username, const QString &password);
    bool isConnected() const;
    void sendJson(const QJsonObject &obj);

signals:
    void loginSuccess();
    void loginFailed(const QString &reason);
    void connectionFailed(const QString &reason); // 新增连接失败信号
    void messageReceived(const QString &msg);

private:
    asio::io_context io_context;
    asio::ip::tcp::socket socket_;
    std::thread io_thread;
    std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> work_guard_;

    void doRead();

    // 用于连续接收数据的缓冲区
    std::shared_ptr<std::string> recvBuffer;
};
