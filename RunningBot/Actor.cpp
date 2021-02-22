#include "Actor.h"
#include "Game.h"

#include <conio.h>
#include <chrono>

namespace actor {

Actor::Actor(const int x, const int y)
	: m_x{x}
	, m_y{y}
{}

const int Actor::GetXPos() const
{
	return m_x;
}

const int Actor::GetYPos() const
{
	return m_y;
}

Player::Player() : Actor(game::Game::s_mapSizeX >> 1, game::Game::s_mapSizeY >> 1) {} // spawn player in the middle

void Player::Move(std::wstring& map)
{
	static const auto removeLastPlayerPos = [&]()
	{
		map[m_x * game::Game::s_mapSizeX + m_y] = ' ';
	};

	if (_kbhit())
	{
		switch (_getch())
		{
		case 'w':
			if (map[(m_x - 1) * game::Game::s_mapSizeX + m_y] != '#')
			{
				removeLastPlayerPos();
				map[--m_x * game::Game::s_mapSizeX + m_y] = 'P';
			}
			break;
		case 'a':
			if (map[m_x * game::Game::s_mapSizeX + m_y - 1] != '#')
			{
				removeLastPlayerPos();
				map[m_x * game::Game::s_mapSizeX + --m_y] = 'P';
			}
			break;
		case 's':
			if (map[(m_x + 1) * game::Game::s_mapSizeX + m_y] != '#')
			{
				removeLastPlayerPos();
				map[++m_x * game::Game::s_mapSizeX + m_y] = 'P';
			}
			break;
		case 'd':
			if (map[m_x * game::Game::s_mapSizeX + m_y + 1] != '#')
			{
				removeLastPlayerPos();
				map[m_x * game::Game::s_mapSizeX + ++m_y] = 'P';
			}
			break;
		default:
			break;
		}
	}
}

Bot::Bot(Player& playerToFollow)
	: Actor(rand() % game::Game::s_mapSizeX, rand() % game::Game::s_mapSizeY) // spawn bot in random place
{
	m_playerToFollow = &playerToFollow;
}

void Bot::Move(std::wstring& map) // TODO: change to Dijkstra's algorithm
{
	static auto timeFirst{std::chrono::system_clock::now()};
	auto timeSecond{std::chrono::system_clock::now()};

	if (std::chrono::duration<float>(timeSecond - timeFirst).count() <= 0.5)
	{
		return;
	}
	timeFirst = timeSecond;

	map[m_x * game::Game::s_mapSizeX + m_y] = ' ';
	if (m_playerToFollow->GetXPos() < m_x)
	{
		--m_x;
	}
	if (m_playerToFollow->GetXPos() > m_x)
	{
		++m_x;
	}
	if (m_playerToFollow->GetYPos() < m_y)
	{
		--m_y;
	}
	if (m_playerToFollow->GetYPos() > m_y)
	{
		++m_y;
	}
	map[m_x * game::Game::s_mapSizeX + m_y] = 'B';
}

} // actor
