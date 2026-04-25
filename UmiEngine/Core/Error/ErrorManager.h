#pragma once
#include <vector>
#include <Entity/Entity.h>
#include <Error/Error.h>
#include <Time/Time.h>

namespace Umi
{
	class Registry;

	class ErrorManager
	{
	private:
		Registry& registry;

		//std::unordered_map<Entity, Timer> errorEntities;

		bool hasFatalError = false;

		void CreateErrorEntity(const Error& error) noexcept;

	public:
		void Update();

		void CreateError(const Error& error) noexcept;

		inline bool HasFatalError() const noexcept { return hasFatalError; }

		ErrorManager(Registry& registry) : registry(registry) {}
	};
}