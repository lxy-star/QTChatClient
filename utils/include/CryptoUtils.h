#pragma once
#include <QString>
#include <QCryptographicHash>
#include <QByteArray>
#include <openssl/aes.h>

class CryptoUtils
{
public:
    // SHA-256哈希（不可逆，用于密码存储）
    static QString sha256(const QString &input);

    // AES-256加密（需密钥和IV）
    static QByteArray aesEncrypt(const QByteArray &data, const QByteArray &key, const QByteArray &iv);

    // AES-256解密
    static QByteArray aesDecrypt(const QByteArray &encrypted, const QByteArray &key, const QByteArray &iv);

    // 从密码和盐派生 AES 密钥和 IV（PBKDF2）
    static void deriveKeyAndIv(const QString &password, const QByteArray &salt, QByteArray &key, QByteArray &iv);
};