#pragma once
#include <string>

namespace Umi
{
	enum class ErrorCode : int
	{
		None = 0,

		//General Errors
		UnknownError,

		//Loading Errors
		FailedToLoadPrefab,
	};

	inline static std::string ErrorCodeToString(ErrorCode code)
	{
		switch (code)
		{
		case ErrorCode::None: return "No error";
		case ErrorCode::UnknownError: return "Unknown error";
		case ErrorCode::FailedToLoadPrefab: return "Failed to load prefab";
		default: return "Unhandled error";
		}
	}

	inline static std::wstring ErrorCodeToWString(ErrorCode code)
	{
		switch (code)
		{
		case ErrorCode::None: return L"No error";
		case ErrorCode::UnknownError: return L"Unknown error";
		case ErrorCode::FailedToLoadPrefab: return L"Failed to load prefab";
		default: return L"Unhandled error";
		}
	}

	struct Error
	{
		ErrorCode code;
		std::string message;
		Error(ErrorCode _code) : code(_code) {}
		Error(ErrorCode _code, const std::string& _message) : code(_code), message(_message) {}
	};

	struct ErrorComponent
	{
		std::string message;

		ErrorComponent() = default;
		ErrorComponent(Error _error) : message(ErrorCodeToString(_error.code)) {}
		ErrorComponent(const std::string& _message) : message(_message) {}
		ErrorComponent(ErrorCode _code) : message(ErrorCodeToString(_code)) {}
	};
}	