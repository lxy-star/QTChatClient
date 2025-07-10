#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    QString getUsername() const;
    QString getPassword() const;
    QString getHost() const;
    quint16 getPort() const;
    QString getMode() const;
    void setUiEnabled(bool enabled);

signals:
    void loginRequested();  // 新增信号

private slots:
    void validateAndRequest();  // 改名，不再调用 accept()

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *hostEdit;
    QLineEdit *portEdit;
    QComboBox *modeBox;
    QPushButton *connectButton;

};

#endif // LOGINDIALOG_H
