#include "zeptogramstate.h"

ZeptoGramState::ZeptoGramState(QObject *parent) : QObject{parent}
{
}

ZeptoGramState* ZeptoGramState::instance()
{
	if (inst == nullptr)
	{
		inst = new ZeptoGramState();
	}

	return inst;
}