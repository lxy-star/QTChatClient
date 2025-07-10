cmake_minimum_required(VERSION 3.16)
project(QtChatClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Enable Qt moc/uic/rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Gui)

# 头文件路径
set(PROJECT_INC ${CMAKE_SOURCE_DIR}/include  ${CMAKE_SOURCE_DIR}/utils)

# 所有源文件和头文件显式列出（推荐做法，Qt 能识别 Q_OBJECT）
set(PROJECT_SRC
    ${CMAKE_SOURCE_DIR}/main.cpp

    ${CMAKE_SOURCE_DIR}/src/ClientSocket.cpp
    ${CMAKE_SOURCE_DIR}/src/LoginDialog.cpp
    ${CMAKE_SOURCE_DIR}/src/mainwindow.cpp
    ${CMAKE_SOURCE_DIR}/src/AppController.cpp
    ${CMAKE_SOURCE_DIR}/src/LoadingDialog.cpp

    ${CMAKE_SOURCE_DIR}/include/ClientSocket.h
    ${CMAKE_SOURCE_DIR}/include/LoginDialog.h
    ${CMAKE_SOURCE_DIR}/include/mainwindow.h
    ${CMAKE_SOURCE_DIR}/include/AppController.h
    ${CMAKE_SOURCE_DIR}/include/LoadingDialog.h

    ${CMAKE_SOURCE_DIR}/utils/crypto.h
)

# 添加可执行文件
add_executable(QtChatClient ${PROJECT_SRC})

qt_add_resources(RESOURCES_ADDED resources.qrc)
target_sources(QtChatClient PRIVATE ${RESOURCES_ADDED})

# 包含头文件目录
target_include_directories(QtChatClient PRIVATE ${PROJECT_INC})

# 链接 Qt 和 ASIO 依赖
target_link_libraries(QtChatClient
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Network
    ws2_32       # Windows sockets for Asio
)
