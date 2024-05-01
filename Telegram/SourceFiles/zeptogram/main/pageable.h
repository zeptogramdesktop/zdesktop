#ifndef PAGEABLE_H
#define PAGEABLE_H

#include <QString>
#include <QDebug>
#include "pagechangeable.h"

#include "zeptogram/zeptogramexecutor.h"

class Pageable
{
public:
    virtual ~Pageable() {}
    void pageChanged();
    virtual QString getPage() = 0;
    PageChangeable* getEmitter();

protected:
    void setEmitter(QObject* parent);
    ZeptoGramExecutor* getZeptoGramExecutor();
    bool isInitialized() const;

    void initPageable(QWidget* widget, Fn<void()> emmitableCallback);
    void registerAsPageableDelegate(QWidget* widget);
    void emitPageChange();

    ZeptoGramExecutor* _executor = ZeptoGramExecutor::instance();

private:
    PageChangeable* _emitter;
    bool _initialized = false;
    Fn<void()> _emmitableCallback;
};

#endif // PAGEABLE_H
