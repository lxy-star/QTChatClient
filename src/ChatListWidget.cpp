#include "ChatListWidget.h"
#include "ClientSocket.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>

ChatListWidget::ChatListWidget(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    chatList_ = new QListWidget(this);
    newChatBtn_ = new QPushButton(" 新建聊天", this);
    logoutBtn_ = new QPushButton("退出登录", this);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(newChatBtn_);
    buttonLayout->addWidget(logoutBtn_);

    layout->addWidget(chatList_);
    layout->addLayout(buttonLayout);

    connect(chatList_, &QListWidget::itemClicked, this, &ChatListWidget::onChatItemClicked);
    connect(newChatBtn_, &QPushButton::clicked, this, &ChatListWidget::onNewChatClicked);
    connect(logoutBtn_, &QPushButton::clicked, this, &ChatListWidget::onLogoutClicked);
}

void ChatListWidget::setClient(ClientSocket *client)
{
    client_ = client;

    connect(client_, &ClientSocket::chatListReceived, this, &ChatListWidget::onReceiveChatList);
    connect(client_, &ClientSocket::verifyUserResult, this, &ChatListWidget::onVerifyResult);
    client_ -> requestChatList();
}

void ChatListWidget::addChatSession(const QString &chatId)
{
    if (chatItems_.contains(chatId))
        return;

    auto *item = new QListWidgetItem(chatId);
    chatList_->addItem(item);
    chatItems_[chatId] = item;
    chatList_->scrollToItem(item);
}

void ChatListWidget::onChatItemClicked(QListWidgetItem *item)
{
    if (item)
        emit chatSelected(item->text());
}

void ChatListWidget::onNewChatClicked()
{
    QStringList options = {"单聊", "群聊"};
    bool ok = false;
    QString choice = QInputDialog::getItem(this, "新建聊天", "请选择聊天类型：", options, 0, false, &ok);
    if (!ok) return;

    QString label = (choice == "群聊") ? "请输入群聊ID：" : "请输入用户名：";
    QString chatId = QInputDialog::getText(this, "新建聊天", label, QLineEdit::Normal, "", &ok);
    if (!ok || chatId.isEmpty()) return;

    // 向服务器验证 chatId 是否存在
    QJsonObject json;
    json["type"] = (choice == "群聊") ? "verify_group" : "verify_user";
    json["target"] = chatId;
    client_->sendJson(json);
}

void ChatListWidget::onVerifyResult(const QString &chatId, bool exists)
{
    if (!exists)
    {
        auto ret = QMessageBox::question(
            this,
            "用户不存在",
            "该用户或群聊不存在，是否继续创建？",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No);

        if (ret == QMessageBox::No)
        {
            // 用户选择取消，不做任何操作
            return;
        }
        // 用户选择继续创建，走下面添加和进入聊天逻辑
    }

    // 验证成功，或用户选择继续创建
    addChatSession(chatId);
    emit chatSelected(chatId);
}


void ChatListWidget::onReceiveChatList(const QJsonArray &chats)
{
    for (const auto &val : chats)
    {
        if (val.isString())
            addChatSession(val.toString());
    }
}

void ChatListWidget::onLogoutClicked()
{
    emit logoutRequested();
}
