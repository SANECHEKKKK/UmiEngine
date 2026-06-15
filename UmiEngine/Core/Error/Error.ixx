module;
#include <stdexcept>
export module Error;

export namespace Umi
{
	struct Error
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
	};
}