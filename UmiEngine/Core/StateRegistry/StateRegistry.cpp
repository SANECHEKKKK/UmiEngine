#include <StateRegistry/StateRegistry.h>

using namespace Umi;

void StateRegistry::Register(StringID stateId, StateDescription stateDescription)
{
	states[stateId] = stateDescription;
}

const StateDescription& StateRegistry::Get(StringID stateId) const
{
	return states.at(stateId);
}