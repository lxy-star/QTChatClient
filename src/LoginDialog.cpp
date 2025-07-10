#include "LoginDialog.h"
#include <QFormLayout>
#include <QIntValidator>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QResizeEvent>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Login/Register");

    usernameEdit = new QLineEdit;
    passwordEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);

    hostEdit = new QLineEdit("127.0.0.1");
    portEdit = new QLineEdit("12345");
    portEdit->setValidator(new QIntValidator(1, 65535, this));

    modeBox = new QComboBox;
    modeBox->addItem("Login");
    modeBox->addItem("Register");

    connectButton = new QPushButton("Connect");

    // 表单布局只放控件，不放按钮
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Username:", usernameEdit);
    formLayout->addRow("Password:", passwordEdit);
    formLayout->addRow("Server IP:", hostEdit);
    formLayout->addRow("Port:", portEdit);
    formLayout->addRow("Mode:", modeBox);

    // 按钮和动画
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(connectButton);
    btnLayout->addStretch(); //  两边撑开，按钮自动居中

    // 主垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);

    mainLayout->addLayout(btnLayout);

    mainLayout->addStretch(); // 底部留白，避免挤压

    setLayout(mainLayout);

    setMinimumSize(300, 200);
    setWindowState(Qt::WindowNoState);

    // 设置控件的sizePolicy，可以让他们水平扩展
    usernameEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    passwordEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    hostEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    portEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    modeBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(connectButton, &QPushButton::clicked, this, &LoginDialog::validateAndRequest);
}

LoginDialog::~LoginDialog() {}

QString LoginDialog::getUsername() const { return usernameEdit->text(); }
QString LoginDialog::getPassword() const { return passwordEdit->text(); }
QString LoginDialog::getHost() const { return hostEdit->text(); }
quint16 LoginDialog::getPort() const { return portEdit->text().toUShort(); }
QString LoginDialog::getMode() const { return modeBox->currentText().toLower(); }

void LoginDialog::validateAndRequest()
{
    if (getUsername().isEmpty() || getPassword().isEmpty() || getHost().isEmpty() || getPort() == 0)
    {
        QMessageBox::warning(this, "Input Error", "Please fill in all fields correctly.");
        return;
    }

    emit loginRequested(); // 由控制器决定是否 accept()
}

void LoginDialog::setUiEnabled(bool enabled)
{
    usernameEdit->setEnabled(enabled);
    passwordEdit->setEnabled(enabled);
    hostEdit->setEnabled(enabled);
    portEdit->setEnabled(enabled);
    modeBox->setEnabled(enabled);
    connectButton->setEnabled(enabled);
}

void LoginDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    int baseFontSize = 10;
    int newFontSize = qMax(baseFontSize, event->size().width() / 40);
    QFont font = this->font();
    font.setPointSize(newFontSize);
    this->setFont(font);
}
