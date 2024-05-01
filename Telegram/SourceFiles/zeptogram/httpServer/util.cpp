#include "util.h"

QString getHttpStatusStr(HttpStatus status)
{
    auto it = httpStatusStrs.find(static_cast<int>(status));
    if (it == httpStatusStrs.end())
        return "";

    return it->second;
}

// no compression
QByteArray gzipCompress(QByteArray &data, int compressionLevel)
{
    return data;
}

// no uncompression
QByteArray gzipUncompress(QByteArray &data)
{
    return data;
}
