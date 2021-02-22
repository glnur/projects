#include <iostream>
#include <string>

#include "Game.h"
#include "Actor.h"

namespace game{

Game::~Game()
{
	delete s_game;
	SetConsoleActiveScreenBuffer(m_consoleOrig);
}

Game* Game::GetInstance()
{
	if (s_game == nullptr)
	{
		s_game = new Game();
	}
	return s_game;
}

bool Game::IfSucceeded() const
{
	return m_succeeded;
}

void Game::Run()
{
	if (!IfSucceeded())
	{
		std::cerr << "Could not start the game" << std::endl;
		return;
	}

	m_map[m_player.GetXPos() * s_mapSizeX + m_player.GetYPos()] = 'P';
	m_map[m_bot.GetXPos() * s_mapSizeX + m_bot.GetYPos()] = 'B';

	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		m_player.Move(m_map);
		m_bot.Move(m_map);
		Draw();
	}
}

void Game::Draw()
{
	DWORD dwBytesWritten = 0;
	WriteConsoleOutputCharacter(m_console, m_map.c_str(), s_mapSizeX * s_mapSizeY, {0,0}, &dwBytesWritten);
}

Game::Game()
	: m_succeeded{false}
	, m_consoleOrig{GetStdHandle(STD_OUTPUT_HANDLE)}
	, m_player{actor::Player()}
	, m_bot{actor::Bot(m_player)}
{
	// Must be s_mapSizeX * s_mapSizeY
	// TODO: add obstacles
	m_map += L"###############" \
			 L"#             #" \
			 L"#             #" \
			 L"#             #" \
			 L"#             #" \
			 L"#             #" \
			 L"#             #" \
			 L"#             #" \
			 L"#             #" \
			 L"#             #" \
			 L"#             #" \
			 L"#             #" \
			 L"#             #" \
			 L"#             #" \
			 L"###############";

	// Assign screen buffer to the console
	m_console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (!SetConsoleActiveScreenBuffer(m_console))
	{
		Error(L"SetConsoleActiveScreenBuffer");
		return;
	}

	m_rectWindow = {0, 0, 1, 1};
	if (!SetConsoleWindowInfo(m_console, TRUE, &m_rectWindow))
	{
		Error(L"First SetConsoleWindowInfo");
		return;
	}

	// Set the size of the screen buffer
	COORD coord = {(short)s_mapSizeX, (short)s_mapSizeY};
	if (!SetConsoleScreenBufferSize(m_console, coord))
	{
		Error(L"SetConsoleScreenBufferSize");
		return;
	}

	CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
	if (!GetConsoleScreenBufferInfo(m_console, &ScreenBufferInfo))
	{
		Error(L"GetConsoleScreenBufferInfo");
		return;
	}
	if (s_mapSizeY > ScreenBufferInfo.dwMaximumWindowSize.Y)
	{
		Error(L"Screen Height is too big");
		return;
	}
	if (s_mapSizeX > ScreenBufferInfo.dwMaximumWindowSize.X)
	{
		Error(L"Screen Width is too big");
		return;
	}

	// Set Physical Console Window Size
	m_rectWindow = {0, 0, (short)s_mapSizeX - 1, (short)s_mapSizeY - 1};
	if (!SetConsoleWindowInfo(m_console, TRUE, &m_rectWindow))
	{
		Error(L"SetConsoleWindowInfo");
		return;
	}

	m_succeeded = true;
}

int Game::Error(const wchar_t* msg) const
{
	wchar_t buf[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
	SetConsoleActiveScreenBuffer(m_consoleOrig);
	wprintf(L"ERROR: %s\n\t%s\n", msg, buf);
	return 0;
}

/*static*/ Game* Game::s_game{nullptr};
/*static*/ int Game::s_mapSizeX{13 + 2/*map border*/};
/*static*/ int Game::s_mapSizeY{13 + 2};

} // game
