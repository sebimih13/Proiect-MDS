#pragma once

#include "../Human/Human.h"
#include "../AIEntity.h"

class Enemy : public virtual Human, public virtual AIEntity
{
protected:


public:

	Enemy(double x, double y, double drawWidth, double drawHeight, double rotateAngle, double speed, double collideWidth, double collideHeight, const std::vector<std::string>& animationsName2D, double health, double stamina, double armor);
	~Enemy();
};