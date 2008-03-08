#include "IoProvider.h"

IOProvider::IOProvider(IOSystem *parent)
{
	parent->RegisterIOProvider(this);
	m_system = parent;
}

IOProvider::~IOProvider(void)
{
	m_system->RemoveIOProvider(this);
}
