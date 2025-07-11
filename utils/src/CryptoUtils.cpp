#include "CryptoUtils.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <QDebug>

QString CryptoUtils::sha256(const QString& input) {
    QByteArray hash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex();
}

QByteArray CryptoUtils::aesEncrypt(const QByteArray& data, const QByteArray& key, const QByteArray& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return QByteArray();

    // 初始化加密操作
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, 
                          reinterpret_cast<const unsigned char*>(key.constData()),
                          reinterpret_cast<const unsigned char*>(iv.constData())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    QByteArray out(data.size() + AES_BLOCK_SIZE, 0);
    int len = 0;
    int outLen = 0;

    // 执行加密
    if (EVP_EncryptUpdate(ctx, 
                         reinterpret_cast<unsigned char*>(out.data()), &len,
                         reinterpret_cast<const unsigned char*>(data.constData()), data.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    outLen = len;

    // 结束加密
    if (EVP_EncryptFinal_ex(ctx, 
                           reinterpret_cast<unsigned char*>(out.data()) + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    outLen += len;
    out.resize(outLen);

    EVP_CIPHER_CTX_free(ctx);
    return out;
}

QByteArray CryptoUtils::aesDecrypt(const QByteArray& encrypted, const QByteArray& key, const QByteArray& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return QByteArray();

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                          reinterpret_cast<const unsigned char*>(key.constData()),
                          reinterpret_cast<const unsigned char*>(iv.constData())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    QByteArray out(encrypted.size(), 0);
    int len = 0;
    int outLen = 0;

    if (EVP_DecryptUpdate(ctx,
                         reinterpret_cast<unsigned char*>(out.data()), &len,
                         reinterpret_cast<const unsigned char*>(encrypted.constData()), encrypted.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    outLen = len;

    if (EVP_DecryptFinal_ex(ctx,
                           reinterpret_cast<unsigned char*>(out.data()) + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    outLen += len;
    out.resize(outLen);

    EVP_CIPHER_CTX_free(ctx);
    return out;
}

void CryptoUtils::deriveKeyAndIv(const QString& password, const QByteArray& salt, QByteArray& key, QByteArray& iv) {
    const int keyLength = 32;  // 256-bit
    const int ivLength  = 16;  // 128-bit
    const int iterations = 10000;

    QByteArray derived;
    derived.resize(keyLength + ivLength);

    int success = PKCS5_PBKDF2_HMAC(
        password.toUtf8().constData(), password.length(),
        reinterpret_cast<const unsigned char*>(salt.constData()), salt.size(),
        iterations,
        EVP_sha256(),
        keyLength + ivLength,
        reinterpret_cast<unsigned char*>(derived.data())
    );

    if (success != 1) {
        qDebug() << "PBKDF2 key derivation failed!";
        return;
    }

    key = derived.left(keyLength);
    iv  = derived.mid(keyLength, ivLength);
}