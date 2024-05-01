#ifndef PAGECHANGEABLE_H
#define PAGECHANGEABLE_H

#include <QObject>

class PageChangeable : public QObject
{
    Q_OBJECT
public:
    explicit PageChangeable(QObject *parent = nullptr);

Q_SIGNALS:
    void pageChanged();
};

#endif // PAGECHANGEABLE_H
