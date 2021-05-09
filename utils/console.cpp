#include <console.h>
#include <Windows.h>

static HANDLE stdout_handle = nullptr;
static HWND   window_handle = nullptr;

con::color::color( con::colors foreground, con::colors background )
	: fg_color(static_cast<std::uint8_t>(foreground)), bg_color(static_cast<std::uint8_t>(background))
{
}

bool con::init()
{
	if ( AllocConsole() )
	{
		FILE *file_ptr;
		freopen_s( &file_ptr, "CONOUT$", "w", stdout );
		freopen_s( &file_ptr, "CONOUT$", "w", stderr );
		freopen_s( &file_ptr, "CONIN$", "r", stdin );
	}

	window_handle = GetConsoleWindow();
	stdout_handle = GetStdHandle( STD_OUTPUT_HANDLE );

	if ( stdout_handle && window_handle )
	{
		con::print();
		return true;
	}
	
	#if _DEBUG
	DebugBreak();
	#endif
	return false;
}

bool con::is_focused()
{
	return window_handle == GetForegroundWindow();
}

void con::print( void )
{
	con::print( con::color() );
}

void con::print( con::color val )
{
	SetConsoleTextAttribute( stdout_handle, val.full );
}

void con::print( con::colors val )
{
	con::print( con::color( val ) );
}

con::log::log(std::wstring_view text, bool newline, const wchar_t def_status[10])
{
	if (newline)
		std::cout << '\n';

	CONSOLE_SCREEN_BUFFER_INFO csbi{};
	GetConsoleScreenBufferInfo(stdout_handle, &csbi);
	this->status_point.x = csbi.dwCursorPosition.X + 1;
	this->status_point.y = csbi.dwCursorPosition.Y;

	con::print(con::colors::BWHITE, "[", con::colors::GRAY, def_status, con::colors::BWHITE, "] ", text);
}

void con::log::status(const wchar_t text[10], con::colors txtcol)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi{};
	GetConsoleScreenBufferInfo(stdout_handle, &csbi);
	SetConsoleCursorPosition(stdout_handle, { this->status_point.x, this->status_point.y });
	con::print(txtcol, text);
	SetConsoleCursorPosition(stdout_handle, csbi.dwCursorPosition);
}

void con::log::success()
{
	this->status(L" SUCCESS ", con::colors::LGREEN);
}

void con::log::error()
{
	this->status(L"  ERROR  ", con::colors::LRED);
}

bool con::log::check(bool result)
{
	if (result)
		this->success();
	else
		this->error();

	return result;
}
