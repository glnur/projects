#ifndef GAME_HEADER
#define GAME_HEADER

#include <string>
#include <windows.h>
#include "Actor.h"

namespace game {

class Game // Singletone
{
public:
	~Game();

	static Game* GetInstance();

	bool IfSucceeded() const;
	void Run();

	Game(Game& other) = delete;
	void operator=(const Game&) = delete;

	static int s_mapSizeX;
	static int s_mapSizeY;

protected:
	void Draw();

	HANDLE m_console;
	HANDLE m_consoleOrig;
	std::wstring m_map;
	SMALL_RECT m_rectWindow;
	actor::Player m_player;
	actor::Bot m_bot;

	static Game* s_game;

private:
	Game();
	int Error(const wchar_t* msg) const;

	bool m_succeeded;
};

} // game

#endif // GAME_HEADER
