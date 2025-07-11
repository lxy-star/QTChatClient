#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QJsonObject>

class ClientSocket;

class ChatDetailWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChatDetailWidget(QWidget *parent = nullptr);
    void setClient(ClientSocket *client);
    void setCurrentChat(const QString &chatId);

signals:
    void backToListRequested();

private slots:
    void onSendClicked();
    void onMessageReceived(const QString &msg);

private:
    ClientSocket *client_ = nullptr;
    QString currentChatId;

    QTextEdit *chatView;
    QLineEdit *inputBox;
    QPushButton *sendButton;
    QPushButton *backButton;
};
