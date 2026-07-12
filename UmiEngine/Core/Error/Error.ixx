module;
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <EngineApi/EngineApi.h>
export module Error;

export namespace Umi
{
	struct ENGINE_API Error
	{
		//--------------FATAL ERROR--------------
		static void FatalError(const char* message) 
		{
			throw std::runtime_error(message);
		}
		static void FatalError(const std::string& message) 
		{
			FatalError(message.c_str());
		}
		//---------------------------------------

		//------------NON FATAL ERROR------------
		static inline std::vector<std::string> NonFatalErrors;

		static void NonFatalError(std::string_view message)
		{
			NonFatalErrors.push_back(std::string(message));
		}
		//---------------------------------------
	};
}