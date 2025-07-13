# QTChatClient
using windows+QT+ASIO+json

## using MSYS2 to build the dependency
```shell
sed -i "s#mirror.msys2.org#mirrors.tuna.tsinghua.edu.cn/msys2#g" /etc/pacman.d/mirrorlist.*
pacman -Syu
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-qt6-base
pacman -S mingw-w64-x86_64-asio
````