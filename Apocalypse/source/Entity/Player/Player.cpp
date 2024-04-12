#include "Player.h"

Player::Player(double x, double y, double drawWidth, double drawHeight, double rotateAngle, double speed, double collideWidth, double collideHeight, const std::vector<std::string>& animationsName2D, double health = 100.0, double stamina = 100.0, double armor = 0.0) :
	Human(x, y, drawWidth, drawHeight, rotateAngle, speed, collideWidth, collideHeight, animationsName2D, health, stamina, armor)
{

}

/*
Player& Player::get()
{
	// TODO: override la niste functii virtuale
	//static Player instance;

	//return instance;
}
*/

Player::~Player()
{

}
