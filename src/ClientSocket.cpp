#include "ClientSocket.h"
#include <QJsonDocument>
#include <QMetaObject>
#include "crypto.h"

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
    obj["type"] = mode; // login 或 register
    obj["username"] = username;
    obj["password"] = sha256(password);
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
    // 这里改用 asio::async_read_until，自动按换行符分割消息更靠谱
    auto buffer = std::make_shared<asio::streambuf>();

    asio::async_read_until(socket_, *buffer, '\n',
                           [this, buffer](std::error_code ec, std::size_t length)
                           {
                               if (!ec)
                               {
                                   // 从 buffer 读取一行消息
                                   std::istream is(buffer.get());
                                   std::string line;
                                   std::getline(is, line);

                                   QString data = QString::fromStdString(line);

                                   // 解析 JSON
                                   QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
                                   if (doc.isObject())
                                   {
                                       auto obj = doc.object();
                                       QString type = obj["type"].toString();

                                       if (type == "login_result")
                                       {
                                           bool success = obj["success"].toBool();
                                           if (success)
                                           {
                                               QMetaObject::invokeMethod(this, "loginSuccess", Qt::QueuedConnection);
                                           }
                                           else
                                           {
                                               QString reason = obj["reason"].toString();
                                               QMetaObject::invokeMethod(this, "loginFailed", Qt::QueuedConnection,
                                                                         Q_ARG(QString, reason));
                                           }
                                       }
                                       else if (type == "chat")
                                       {
                                           QString from = obj["from"].toString();
                                           QString msg = obj["message"].toString();
                                           emit messageReceived(QString("%1: %2").arg(from, msg));
                                       }
                                   }

                                   // 继续读取下一条消息
                                   doRead();
                               }
                               else
                               {
                                   // 连接断开或出错时，可以选择发信号通知 UI
                                   QMetaObject::invokeMethod(this, "connectionFailed", Qt::QueuedConnection,
                                                             Q_ARG(QString, QString("Disconnected from server")));
                               }
                           });
}

bool ClientSocket::isConnected() const
{
    return socket_.is_open();
}
