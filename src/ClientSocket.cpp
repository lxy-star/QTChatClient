#include "ClientSocket.h"
#include <QJsonDocument>
#include <QMetaObject>
#include <QThread>
#include "CryptoUtils.h"

// .cpp 构造函数
ClientSocket::ClientSocket(QObject *parent)
    : QObject(parent),
      socket_(io_context),
      recvBuffer(std::make_shared<std::string>())
{
    work_guard_ = std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(asio::make_work_guard(io_context));
}

ClientSocket::~ClientSocket()
{
    qDebug() << "~ClientSocket() start";

    work_guard_.reset();
    io_context.stop();

    if (io_thread.joinable()) {
        qDebug() << "Waiting for IO thread to exit...";
        io_thread.join();
    }

    qDebug() << "~ClientSocket() completed";
}


void ClientSocket::connectToServer(const QString &host, quint16 port, std::function<void()> onConnected)
{
    qDebug() << "Login connectToServer started";

    // 1. 启动 IO 线程（只启动一次）
    if (!io_thread.joinable())
    {
        io_thread = std::thread([this]()
                                {
            qDebug() << "IO thread started";
            io_context.run();
            qDebug() << "io_context.run in thread:" << QThread::currentThread();
            qDebug() << "IO thread finished"; });
    }

    // 2. Post connect 到 io_context，让 async_connect 确保在 IO 线程中注册
    io_context.post([=]()
                    {
        qDebug() << "lxy Post connect";
        asio::ip::tcp::endpoint ep(asio::ip::make_address(host.toStdString()), port);
        socket_.async_connect(ep, [this, onConnected](std::error_code ec) {
            qDebug() << "Connect callback triggered";
            if (!ec) {
                qDebug() << "Connected successfully";
                doRead();
                if (onConnected) onConnected();
            } else {
                qDebug() << "Connect failed:" << QString::fromLocal8Bit(ec.message().c_str());
                emit connectionFailed(QString("Failed to connect: %1").arg(QString::fromLocal8Bit(ec.message().c_str())));
            }
        }); });
    qDebug() << "connectToServer called from thread:" << QThread::currentThread();
}

void ClientSocket::loginOrRegister(const QString &mode, const QString &username, const QString &password)
{
    QJsonObject obj;
    username_ = username;
    obj["type"] = mode; // login 或 register
    obj["username"] = username;
    // obj["password"] = CryptoUtils::sha256(password);
    obj["password"] = password;
    sendJson(obj);
}

void ClientSocket::loginOut(){
    QJsonObject obj;
    obj["type"] = "logout";
    obj["username"] = username_;
    sendJson(obj);
}

void ClientSocket::sendJson(const QJsonObject &obj)
{
    QJsonDocument doc(obj);
    QByteArray msg = doc.toJson(QJsonDocument::Compact);
    msg.append('\n'); // 以换行符作为消息分隔符

    // 注意：asio::async_write的回调必须持有this，避免提前销毁
    asio::async_write(socket_, asio::buffer(msg), [this](std::error_code ec, std::size_t /*length*/)
                      {
        if (ec) {
            // 可以在这里处理写错误
        } });
}

void ClientSocket::doRead()
{
    auto buffer = std::make_shared<asio::streambuf>();

    asio::async_read_until(socket_, *buffer, '\n',
        [this, buffer](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                std::istream is(buffer.get());
                std::string line;
                std::getline(is, line);

                QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(line).toUtf8());

                if (doc.isObject())
                {
                    handleJsonObject(doc.object());
                }

                doRead();
            }
            else
            {
                QMetaObject::invokeMethod(this, "connectionFailed", Qt::QueuedConnection,
                                          Q_ARG(QString, QString("Disconnected from server")));
            }
        });
}

void ClientSocket::handleJsonObject(const QJsonObject &obj)
{
    QString type = obj["type"].toString();
    qDebug()<<"lxy handleJsonObject type: " + type;
    if (type == "login_result")
    {
        bool success = obj["success"].toBool();
        if (success)
            QMetaObject::invokeMethod(this, "loginSuccess", Qt::QueuedConnection);
        else
            QMetaObject::invokeMethod(this, "loginFailed", Qt::QueuedConnection, Q_ARG(QString, obj["reason"].toString()));
    }
    else if (type == "register_result")
    {
        bool success = obj["success"].toBool();
        if (success)
            QMetaObject::invokeMethod(this, "registerSuccess", Qt::QueuedConnection);
        else
            QMetaObject::invokeMethod(this, "registerFailed", Qt::QueuedConnection, Q_ARG(QString, obj["reason"].toString()));
    }
    else if (type == "chat")
    {
        QString from = obj["from"].toString();
        QString msg = obj["message"].toString();
        emit messageReceived(QString("%1: %2").arg(from, msg));
    }
    else if (type == "chat_list")
    {
        QJsonArray chats = obj["chats"].toArray();
        emit chatListReceived(chats);
    }
    else if (type == "verify_result")
    {
        QString target = obj["target"].toString();
        bool exists = obj["exists"].toBool();
        emit verifyUserResult(target, exists);
    }else if(type == "logout_result"){
        bool success = obj["success"].toBool();
        QMetaObject::invokeMethod(this, "logoutSuccess", Qt::QueuedConnection);
    }
}


bool ClientSocket::isConnected() const
{
    return socket_.is_open();
}

void ClientSocket::requestChatList()
{
    QJsonObject obj;
    obj["type"] = "get_chat_list";
    sendJson(obj);
}

void ClientSocket::verifyTargetExistence(const QString &type, const QString &target)
{
    QJsonObject obj;
    obj["type"] = (type == "group") ? "verify_group" : "verify_user";
    obj["target"] = target;
    sendJson(obj);
}