#ifndef SEALHAT_DESERIALIZE_H
#define SEALHAT_DESERIALIZE_H

#include <QObject>
#include <QFile>
#include "seal_Types.h"

class sealhat_deserialize : public QObject
{
        Q_OBJECT
    public:
        explicit sealhat_deserialize(QObject *parent = nullptr);

        void setFileOutput(QFile* outfile);

    signals:

    public slots:
        void serialReceive(QByteArray data);
};

#endif // SEALHAT_DESERIALIZE_H
