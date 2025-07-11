cmake_minimum_required(VERSION 3.16)
project(QtChatClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Enable Qt moc/uic/rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Gui)

# OpenSSL
find_package(OpenSSL REQUIRED)

# 头文件路径
set(PROJECT_INC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/utils/include
)

# 所有源文件
set(PROJECT_SRC
    ${CMAKE_SOURCE_DIR}/main.cpp

    ${CMAKE_SOURCE_DIR}/src/ClientSocket.cpp
    ${CMAKE_SOURCE_DIR}/src/LoginDialog.cpp
    ${CMAKE_SOURCE_DIR}/src/ChatDetailWidget.cpp
    ${CMAKE_SOURCE_DIR}/src/AppController.cpp
    ${CMAKE_SOURCE_DIR}/src/LoadingDialog.cpp
    ${CMAKE_SOURCE_DIR}/src/ChatListWidget.cpp

    ${CMAKE_SOURCE_DIR}/utils/src/CryptoUtils.cpp

    ${CMAKE_SOURCE_DIR}/include/ClientSocket.h
    ${CMAKE_SOURCE_DIR}/include/LoginDialog.h
    ${CMAKE_SOURCE_DIR}/include/ChatDetailWidget.h
    ${CMAKE_SOURCE_DIR}/include/AppController.h
    ${CMAKE_SOURCE_DIR}/include/LoadingDialog.h
    ${CMAKE_SOURCE_DIR}/include/ChatListWidget.h

    ${CMAKE_SOURCE_DIR}/utils/include/CryptoUtils.h
)

# 添加可执行文件
add_executable(QtChatClient ${PROJECT_SRC})

# Qt 资源
qt_add_resources(RESOURCES_ADDED resources.qrc)
target_sources(QtChatClient PRIVATE ${RESOURCES_ADDED})

# 包含头文件目录
target_include_directories(QtChatClient PRIVATE ${PROJECT_INC})

# 链接 Qt + ASIO + OpenSSL
target_link_libraries(QtChatClient
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Network
    OpenSSL::SSL
    OpenSSL::Crypto
    ws2_32           # 仅 Windows 使用 asio 时所需
)
