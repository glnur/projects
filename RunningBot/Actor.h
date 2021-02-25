#ifndef PLAYER_HEADER
#define PLAYER_HEADER

#include <string>
#include <memory>

namespace actor {

class Actor
{
public:
	Actor(const int x, const int y);

	virtual void Move(std::wstring& map) = 0;

	const int GetXPos() const;
	const int GetYPos() const;

protected:
	int m_x, m_y;
	static char s_charToShow;
};

class Bot;

class Player : public Actor
{
public:
	Player();

	void Swap(Bot& bot, std::wstring& map);
	void Move(std::wstring& map) final;

protected:
	static char s_charToShow;
};

class Bot : public Actor
{
public:
	Bot(Player& playerToFollow);

	void Move(std::wstring& map) final;

	friend Player;

protected:
	std::unique_ptr<Player> m_playerToFollow;
	static char s_charToShow;
};

} // actor

#endif // PLAYER_HEADER
