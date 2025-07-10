#include <QApplication>
#include "AppController.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 一定要创建在堆上，生命周期受 QApplication 控制
    auto controller = new AppController;
    controller->start();  // 显示登录窗口

    return app.exec();    // 必须调用，Qt 事件循环才能工作
}
