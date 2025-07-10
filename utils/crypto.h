#pragma once
#include <QString>
#include <QCryptographicHash>

inline QString sha256(const QString& input) {
    QByteArray hash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex();
}
