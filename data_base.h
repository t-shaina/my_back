#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <QObject>

class Data_base : public QObject
{
    Q_OBJECT
public:
    explicit Data_base(QObject *parent = nullptr);

signals:

};

#endif // DATA_BASE_H
