#pragma once
#include <unordered_map>
#include <StateRegistry/StateDescription.h>
#include <StateRegistry/StateID.h>

namespace Umi
{
	class StateRegistry
	{
	private:
		std::unordered_map<StringID, StateDescription> states;

	public:
		void Register(StringID stateId, StateDescription stateDescription );
		const StateDescription& Get(StringID stateId) const;
	};
}