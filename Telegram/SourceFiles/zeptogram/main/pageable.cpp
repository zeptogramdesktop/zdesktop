#include "pageable.h"

void Pageable::pageChanged()
{
	if (!isInitialized())
	{
		qDebug() << "ZPT: This " << this << " widget is not initialized as PageableDelegate";
		return;
	}

	auto emitter = getEmitter();
	if (emitter == NULL)
	{
		qDebug() << "ZPT: Emitter is null, cannot emit pageChanged(), possibly not initialized as PageableDelegate";
		return;
	}

	// cannot emit in non QObject!
	// so we need to call callback
	//Q_EMIT emitter->pageChanged();
	emitPageChange();
}

PageChangeable* Pageable::getEmitter()
{
	return this->_emitter;
}

void Pageable::setEmitter(QObject* parent)
{
	this->_emitter = new PageChangeable(parent);
}

ZeptoGramExecutor* Pageable::getZeptoGramExecutor()
{
	if (_executor == NULL)
	{
		_executor = ZeptoGramExecutor::instance();
	}
	return _executor;
}

bool Pageable::isInitialized() const
{
	return _initialized;
}

void Pageable::initPageable(QWidget* widget, Fn<void()> emmitableCallback)
{
	setEmitter(widget);
	registerAsPageableDelegate(widget);
	_initialized = true;
	_emmitableCallback = emmitableCallback;
}

void Pageable::registerAsPageableDelegate(QWidget* widget)
{
	if (_executor == NULL)
	{
		_executor = ZeptoGramExecutor::instance();
	}

	_executor->registerPageable(UiDelegate(getPage(), widget));
}

void Pageable::emitPageChange()
{
	if (_emmitableCallback == NULL)
	{
		qDebug() << "Cannot emit pageChanged, _emmitableCallback is NULL, possibly incorrect initialization";
		return;
	}

	_emmitableCallback();
}
