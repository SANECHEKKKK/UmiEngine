#include <windows.h>
#include <sstream>
#include <Editor/Editor.h>

#define DBOUT( s )            \
{                             \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugStringW( os_.str().c_str() );  \
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
	try
	{
		Editor editor(hInstance, "Umi Editor");
		editor.Run();
	}
	catch (const std::exception& e)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {}

		MessageBoxA(nullptr, e.what(), "Exception", MB_ICONEXCLAMATION | MB_OK);
		DBOUT(e.what());
	}
	catch (...)
	{
		MessageBoxA(nullptr, "Details unavailable", "Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return 0;
}