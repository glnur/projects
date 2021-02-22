#ifndef PLAYER_HEADER
#define PLAYER_HEADER

#include <string>

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
};

class Player : public Actor
{
public:
	Player();

	void Move(std::wstring& map) final;
};

class Bot : public Actor
{
public:
	Bot(Player& playerToFollow);

	void Move(std::wstring& map) final;

protected:
	Player* m_playerToFollow;
};

} // actor

#endif // PLAYER_HEADER
