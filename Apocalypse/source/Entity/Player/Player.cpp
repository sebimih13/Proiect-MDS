#include "Player.h"

#include <iostream> // TODO: debug
#include <fstream>
#include <memory>
#include <cmath>

#include <nlohmann/json.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../WindowManager/WindowManager.h"
#include "../../ResourceManager/ResourceManager.h"
#include "../../Input/InputHandler.h"
#include "../../GlobalClock/GlobalClock.h"
#include "../../Camera/Camera.h"
#include "../../Renderer/SpriteRenderer.h"
#include "../../SoundManager/SoundManager.h"
#include "../../MenuManager/PauseMenu/PauseMenu.h"
#include "../../Game/Game.h"
#include "../Bullet/Bullet.h"
#include "../Wall/Wall.h"
#include "../Door/Door.h"
#include "../../MenuManager/MenuManager.h"
#include "../../MenuManager/ShopMenu/ShopMenu.h"

Player::Player(double x, double y, double drawWidth, double drawHeight, double rotateAngle, double speed, double collideWidth, double collideHeight, const std::map<AnimatedEntity::EntityStatus, std::string>& animationsName2D, const std::vector<EntityStatus>& statuses, double runningSpeed, double health = 100.0, double stamina = 100.0, double armor = 0.0) :
	Entity(x, y, drawWidth, drawHeight, rotateAngle, speed),
	CollidableEntity(x, y, drawWidth, drawHeight, rotateAngle, speed, collideWidth, collideHeight),
	AnimatedEntity(x, y, drawWidth, drawHeight, rotateAngle, speed, animationsName2D, statuses),
	Human(x, y, drawWidth, drawHeight, rotateAngle, speed, collideWidth, collideHeight, animationsName2D, statuses, health),
	runningSpeed(runningSpeed), stamina(stamina), armor(armor), armorCap(100.0), staminaChangeSpeed(50.0), staminaCap(100.0), gold(0), goldCap(9999999), // TODO: mai frumos pt goldCap se poate?
	moveUpUsed(false), moveDownUsed(false), moveRightUsed(false), moveLeftUsed(false), runUsed(false), interactUsed(false),
	walkingOffsetSize(0.01), runningOffsetSize(0.05),
	walkingOffsetSpeed(10.0), runningOffsetSpeed(15.0),
	weapons({ std::make_shared<Weapon>(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "fist0", 0.0, 0.0, 0.0, 1, 0.0, Weapon::WeaponType::FIST, 0.0, "", "", "")
		, std::make_shared<Weapon>(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "pistol0", 0.0, 0.0, 0.5, 20, 5.0, Weapon::WeaponType::REVOLVER, 0.0, "revolverReload", "revolverDraw", "revolverEmpty")
		, nullptr
		, nullptr
		, nullptr
		, nullptr}),
	currentWeaponIndex(0)
{
	// TODO: test
	bullets[Weapon::WeaponType::REVOLVER] = 1024;
}

Player& Player::get()
{
	static Player instance(10.5, 10.5, 1.0, 1.0, 0.0, 5.0, 0.4, 0.4, std::map<AnimatedEntity::EntityStatus, std::string>
	{
		   { EntityStatus::ARMS_HOLDING_GRENADE, "player0ArmsHoldingGrenade" },
		   { EntityStatus::ARMS_HOLDING_KNIFE, "player0ArmsHoldingKnife" },
		   { EntityStatus::ARMS_HOLDING_PISTOL, "player0ArmsHoldingPistol" },
		   { EntityStatus::ARMS_MOVING_AHEAD, "player0ArmsMovingAhead" },
		   { EntityStatus::ARMS_MOVING_AROUND_WALKING, "player0ArmsMovingAroundWalking"},
		   { EntityStatus::ARMS_MOVING_AROUND_RUNNING, "player0ArmsMovingAroundRunning"},
		   { EntityStatus::ARMS_NOT, "player0ArmsNot"},
		   { EntityStatus::ARMS_RELOADING_PISTOL, "player0ArmsReloadingPistol"},
		   { EntityStatus::ARMS_USING_GRENADE, "player0ArmsUsingGrenade"},
		   { EntityStatus::ARMS_USING_KNIFE, "player0ArmsUsingKnife"},
		   { EntityStatus::ARMS_USING_PISTOL, "player0ArmsUsingPistol"},
		   { EntityStatus::BODY_IDLE, "player0BodyIdle"},
		   { EntityStatus::HEAD_ANGRY, "player0HeadAngry"},
		   { EntityStatus::HEAD_IDLE, "player0HeadIdle"},
		   { EntityStatus::HEAD_SATISFIED, "player0HeadSatisfied"},
		   { EntityStatus::HEAD_TIRED, "player0HeadTired"},
		   { EntityStatus::LEGS_MOVING_AROUND, "player0LegsMovingAround"},
		   { EntityStatus::LEGS_NOT, "player0LegsNot"}
	},
		{
			EntityStatus::LEGS_NOT,
			EntityStatus::ARMS_MOVING_AHEAD,
			EntityStatus::BODY_IDLE,
			EntityStatus::HEAD_IDLE
		}
	, 7.5);

	return instance;
}

void Player::onCollide(CollidableEntity& other, glm::vec2 overlap)
{
	// TODO: nu e totul implementat

	if (dynamic_cast<Wall*>(&other) != nullptr)
	{
		if (overlap.x < overlap.y)
		{
			if (this->x < other.getX())
				this->x -= (overlap.x + CollidableEntity::EPS);
			else
				this->x += (overlap.x + CollidableEntity::EPS);
		}
		else
		{
			if (this->y < other.getY())
				this->y -= (overlap.y + CollidableEntity::EPS);
			else
				this->y += (overlap.y + CollidableEntity::EPS);
		}
	}
	else if (dynamic_cast<Door*>(&other) != nullptr)
	{
		if (overlap.x < overlap.y)
		{
			if (this->x < other.getX())
				this->x -= (overlap.x + CollidableEntity::EPS);
			else
				this->x += (overlap.x + CollidableEntity::EPS);
		}
		else
		{
			if (this->y < other.getY())
				this->y -= (overlap.y + CollidableEntity::EPS);
			else
				this->y += (overlap.y + CollidableEntity::EPS);
		}
	}
	else if (dynamic_cast<CollidableEntity*>(&other) != nullptr) // TODO: aici intra in calcul si bullets !!!!
	{
		if (overlap.x < overlap.y)
		{
			if (this->x < other.getX())
				this->x -= (overlap.x + CollidableEntity::EPS) / 2.0;
			else
				this->x += (overlap.x + CollidableEntity::EPS) / 2.0;
		}
		else
		{
			if (this->y < other.getY())
				this->y -= (overlap.y + CollidableEntity::EPS) / 2.0;
			else
				this->y += (overlap.y + CollidableEntity::EPS) / 2.0;
		}
	}
}

Player::~Player()
{

}

void Player::update()
{
	// weapon
	this->weapons[this->currentWeaponIndex]->update();

	// head
	if (this->statuses[3] == EntityStatus::HEAD_TIRED)
	{
		this->stamina += this->staminaChangeSpeed * GlobalClock::get().getDeltaTime();
		this->stamina = std::min(this->stamina, this->staminaCap);

		if (this->stamina == this->staminaCap)
			this->statuses[3] = EntityStatus::HEAD_IDLE;
	}

	if (this->statuses[3] == EntityStatus::HEAD_TIRED)
	{
		this->statuses[0] = EntityStatus::LEGS_NOT;

		if (this->statuses[1] == EntityStatus::ARMS_MOVING_AHEAD ||
			this->statuses[1] == EntityStatus::ARMS_MOVING_AROUND_WALKING ||
			this->statuses[1] == EntityStatus::ARMS_MOVING_AROUND_RUNNING)
			this->statuses[1] = EntityStatus::ARMS_NOT;

		this->statuses[2] = EntityStatus::BODY_IDLE;

		return;
	}

	// body (nu avem nimic, o singura animatie de un frame, atat)

	// arms
	if (this->moveUpUsed == true || this->moveDownUsed == true
		|| this->moveRightUsed == true || this->moveLeftUsed == true)
	{
		if (this->runUsed == false)
		{
			this->statuses[0] = EntityStatus::LEGS_NOT;
			if (this->statuses[1] == EntityStatus::ARMS_MOVING_AHEAD ||
				this->statuses[1] == EntityStatus::ARMS_MOVING_AROUND_WALKING ||
				this->statuses[1] == EntityStatus::ARMS_MOVING_AROUND_RUNNING)
				this->statuses[1] = EntityStatus::ARMS_MOVING_AROUND_WALKING;
			this->statuses[2] = EntityStatus::BODY_IDLE;
		}
		else
		{
			this->statuses[0] = EntityStatus::LEGS_MOVING_AROUND;
			if (this->statuses[1] == EntityStatus::ARMS_MOVING_AHEAD ||
				this->statuses[1] == EntityStatus::ARMS_MOVING_AROUND_WALKING ||
				this->statuses[1] == EntityStatus::ARMS_MOVING_AROUND_RUNNING)
				this->statuses[1] = EntityStatus::ARMS_MOVING_AROUND_RUNNING;
			this->statuses[2] = EntityStatus::BODY_IDLE;

			this->stamina -= this->staminaChangeSpeed * GlobalClock::get().getDeltaTime();
			this->stamina = std::max(0.0, this->stamina);

			if (this->stamina == 0.0)
			{
				this->statuses[3] = EntityStatus::HEAD_TIRED;
			}
		}
	}
	else // IDLE
	{
		this->statuses[0] = EntityStatus::LEGS_NOT;
		if (this->statuses[1] == EntityStatus::ARMS_MOVING_AHEAD ||
			this->statuses[1] == EntityStatus::ARMS_MOVING_AROUND_WALKING ||
			this->statuses[1] == EntityStatus::ARMS_MOVING_AROUND_RUNNING)
			this->statuses[1] = EntityStatus::ARMS_MOVING_AHEAD;
		this->statuses[2] = EntityStatus::BODY_IDLE;

		this->stamina += this->staminaChangeSpeed * GlobalClock::get().getDeltaTime();
		this->stamina = std::min(this->stamina, this->staminaCap);
	}

	if (this->weapons[this->currentWeaponIndex]->getWeaponType() == Weapon::WeaponType::FIST)
	{
		if (this->statuses[1] != EntityStatus::ARMS_MOVING_AHEAD &&
			this->statuses[1] != EntityStatus::ARMS_MOVING_AROUND_WALKING &&
			this->statuses[1] != EntityStatus::ARMS_MOVING_AROUND_RUNNING)
				this->statuses[1] = EntityStatus::ARMS_MOVING_AHEAD;
	}
	else if (this->weapons[this->currentWeaponIndex]->getWeaponType() == Weapon::WeaponType::REVOLVER)
	{
		if (!this->weapons[this->currentWeaponIndex]->stillReloading())
			this->statuses[1] = EntityStatus::ARMS_HOLDING_PISTOL;
		else
			this->statuses[1] = EntityStatus::ARMS_RELOADING_PISTOL;
	}

	// Sound
	switch (this->statuses[1])
	{
	case EntityStatus::ARMS_MOVING_AHEAD:
		SoundManager::get().pause("walking");
		SoundManager::get().pause("running");
		break;

	case EntityStatus::ARMS_MOVING_AROUND_WALKING:
		SoundManager::get().resume("walking");
		SoundManager::get().pause("running");
		break;

	case EntityStatus::ARMS_MOVING_AROUND_RUNNING:
		SoundManager::get().pause("walking");
		SoundManager::get().resume("running");
		break;
	}

	switch (this->statuses[3])
	{
	case EntityStatus::HEAD_TIRED:
		SoundManager::get().pause("walking");
		SoundManager::get().pause("running");
		break;

	//case EntityStatus::DYING:
	//	SoundManager::get().pause("walking");
	//	SoundManager::get().pause("running");
	//	break;

	default:
		SoundManager::get().pause("walking");
		SoundManager::get().pause("running");
		break;
	}

	if (this->statuses[3] == EntityStatus::HEAD_TIRED)
	{
		return;
	}

	double currentSpeed = this->speed;

	if (this->runUsed)
		currentSpeed = this->runningSpeed;

	double xOffset = 0.0;
	double yOffset = 0.0;

	if (this->moveRightUsed)
		xOffset += currentSpeed * GlobalClock::get().getDeltaTime();
	if (this->moveLeftUsed)
		xOffset -= currentSpeed * GlobalClock::get().getDeltaTime();
	if (this->moveUpUsed)
		yOffset += currentSpeed * GlobalClock::get().getDeltaTime();
	if (this->moveDownUsed)
		yOffset -= currentSpeed * GlobalClock::get().getDeltaTime();

	if (xOffset != 0.0 && yOffset != 0.0)
	{
		xOffset /= glm::sqrt(2.0);
		yOffset /= glm::sqrt(2.0);
	}

	this->x += xOffset;
	this->y += yOffset;
}

void Player::setupPlayerInputComponent()
{
	InputHandler::getPlayerInputComponent().bindAction("MOVE_UP", InputEvent::IE_Pressed, std::bind(&Player::moveUp, this));
	InputHandler::getPlayerInputComponent().bindAction("MOVE_UP", InputEvent::IE_Released, std::bind(&Player::moveUpReleased, this));

	InputHandler::getPlayerInputComponent().bindAction("MOVE_DOWN", InputEvent::IE_Pressed, std::bind(&Player::moveDown, this));
	InputHandler::getPlayerInputComponent().bindAction("MOVE_DOWN", InputEvent::IE_Released, std::bind(&Player::moveDownReleased, this));

	InputHandler::getPlayerInputComponent().bindAction("MOVE_RIGHT", InputEvent::IE_Pressed, std::bind(&Player::moveRight, this));
	InputHandler::getPlayerInputComponent().bindAction("MOVE_RIGHT", InputEvent::IE_Released, std::bind(&Player::moveRightReleased, this));

	InputHandler::getPlayerInputComponent().bindAction("MOVE_LEFT", InputEvent::IE_Pressed, std::bind(&Player::moveLeft, this));
	InputHandler::getPlayerInputComponent().bindAction("MOVE_LEFT", InputEvent::IE_Released, std::bind(&Player::moveLeftReleased, this));

	InputHandler::getPlayerInputComponent().bindAction("RUN", InputEvent::IE_Pressed, std::bind(&Player::run, this));
	InputHandler::getPlayerInputComponent().bindAction("RUN", InputEvent::IE_Released, std::bind(&Player::runReleased, this));

	InputHandler::getPlayerInputComponent().bindAction("SHOOT", InputEvent::IE_Pressed, std::bind(&Player::shoot, this));
	InputHandler::getPlayerInputComponent().bindAction("RELOAD", InputEvent::IE_Pressed, std::bind(&Player::reload, this));

	InputHandler::getPlayerInputComponent().bindAxis(std::bind(&Player::look, this, std::placeholders::_1, std::placeholders::_2));

	InputHandler::getPlayerInputComponent().bindAction("PAUSE", InputEvent::IE_Pressed, std::bind(&Player::pauseGame, this));

	InputHandler::getPlayerInputComponent().bindAction("INTERACT", InputEvent::IE_Pressed, std::bind(&Player::interact, this));
	InputHandler::getPlayerInputComponent().bindAction("INTERACT", InputEvent::IE_Released, std::bind(&Player::interactReleased, this));

	InputHandler::getPlayerInputComponent().bindAction("WEAPON_SLOT_1", InputEvent::IE_Pressed, std::bind(&Player::weaponSlot1, this));
	InputHandler::getPlayerInputComponent().bindAction("WEAPON_SLOT_2", InputEvent::IE_Pressed, std::bind(&Player::weaponSlot2, this));
	InputHandler::getPlayerInputComponent().bindAction("WEAPON_SLOT_3", InputEvent::IE_Pressed, std::bind(&Player::weaponSlot3, this));
	InputHandler::getPlayerInputComponent().bindAction("WEAPON_SLOT_4", InputEvent::IE_Pressed, std::bind(&Player::weaponSlot4, this));
	InputHandler::getPlayerInputComponent().bindAction("WEAPON_SLOT_5", InputEvent::IE_Pressed, std::bind(&Player::weaponSlot5, this));
	InputHandler::getPlayerInputComponent().bindAction("WEAPON_SLOT_6", InputEvent::IE_Pressed, std::bind(&Player::weaponSlot6, this));

	// shop test
	InputHandler::getPlayerInputComponent().bindAction("SHOP", InputEvent::IE_Pressed, std::bind(&Player::enterShop, this));
}

void Player::moveUp()
{
	this->moveUpUsed = true;
}

void Player::moveDown()
{
	this->moveDownUsed = true;
}

void Player::moveRight()
{
	this->moveRightUsed = true;
}

void Player::moveLeft()
{
	this->moveLeftUsed = true;
}

void Player::run()
{
	this->runUsed = true;
}

void Player::interact()
{
	this->interactUsed = true;
}

void Player::moveUpReleased()
{
	this->moveUpUsed = false;
}

void Player::moveDownReleased()
{
	this->moveDownUsed = false;
}

void Player::moveRightReleased()
{
	this->moveRightUsed = false;
}

void Player::moveLeftReleased()
{
	this->moveLeftUsed = false;
}

void Player::runReleased()
{
	this->runUsed = false;
}

void Player::interactReleased()
{
	this->interactUsed = false;
}

void Player::shoot()
{
	this->weapons[this->currentWeaponIndex]->onClick();
}

void Player::reload()
{
	std::cout << "RELOAD" << std::endl;
	this->weapons[this->currentWeaponIndex]->reload();
}

void Player::look(double xpos, double ypos)
{
	glm::vec2 posCenter = Camera::get().screenPosition(this->x, this->y) + glm::vec2(WindowManager::get().getWindowWidth() / 2.0, WindowManager::get().getWindowHeight() / 2.0);
	double xCenter = posCenter.x;
	double yCenter = posCenter.y;

	double xLungime = std::abs(xpos - xCenter);
	double yLungime = std::abs(ypos - yCenter);

	// cadran raportat la centru
	if (xpos > xCenter && ypos < yCenter) // cadran I
	{
		this->rotateAngle = glm::degrees(glm::atan(yLungime / xLungime));
	}
	else if (xpos < xCenter && ypos < yCenter) // cadran II
	{
		this->rotateAngle = 90.0 + glm::degrees(glm::atan(xLungime / yLungime));
	}
	else if (xpos < xCenter && ypos > yCenter) // cadran III
	{
		this->rotateAngle = 180.0 + glm::degrees(glm::atan(yLungime / xLungime));
	}
	else if (xpos > xCenter && ypos > yCenter) // cadran IV
	{
		this->rotateAngle = 270.0 + glm::degrees(glm::atan(xLungime / yLungime));
	}
}

void Player::pauseGame()
{
	MenuManager::get().push(PauseMenu::get());
	// PauseMenu::get().setIsInGame(false);
	// PauseMenu::get().setupPauseMenuInputComponent();
	InputHandler::setInputComponent(InputHandler::getMenuInputComponent());
}

void Player::weaponSlot1()
{
	if (this->weapons[0] != nullptr)
	{
		weapons[0]->drawWeapon();
		this->currentWeaponIndex = 0;
	}
}

void Player::weaponSlot2()
{
	if (this->weapons[1] != nullptr)
	{
		weapons[1]->drawWeapon();
		this->currentWeaponIndex = 1;
	}
}

void Player::weaponSlot3()
{
	if (this->weapons[2] != nullptr)
	{
		weapons[2]->drawWeapon();
		this->currentWeaponIndex = 2;
	}
}

void Player::weaponSlot4()
{
	if (this->weapons[3] != nullptr)
	{
		weapons[3]->drawWeapon();
		this->currentWeaponIndex = 3;
	}
}

void Player::weaponSlot5()
{
	if (this->weapons[4] != nullptr)
	{
		weapons[4]->drawWeapon();
		this->currentWeaponIndex = 4;
	}
}

void Player::weaponSlot6()
{
	if (this->weapons[5] != nullptr)
	{
		weapons[5]->drawWeapon();
		this->currentWeaponIndex = 5;
	}
}

void Player::draw()
{
	// TODO: (pana miercuri) boolene isRunning si isWalking + de rezolvat bug la reload + offset cu sin pentru zombie cand merg
	if (this->statuses[1] == EntityStatus::ARMS_MOVING_AROUND_WALKING)
	{
		for (int i = 0; i < this->statuses.size(); ++i)
			SpriteRenderer::get().draw(ResourceManager::getShader("sprite"), ResourceManager::getFlipbook(this->animationsName2D[this->statuses[i]]).getTextureAtTime(GlobalClock::get().getCurrentTime() - this->timesSinceStatuses[i]), Camera::get().screenPosition(this->x, this->y), Camera::get().screenSize(this->drawWidth + this->walkingOffsetSize * glm::sin(this->walkingOffsetSpeed * (GlobalClock::get().getCurrentTime() - this->timesSinceStatuses[i])), this->drawHeight + this->walkingOffsetSize * glm::sin(this->walkingOffsetSpeed * (GlobalClock::get().getCurrentTime() - this->timesSinceStatuses[i]))), this->rotateAngle);
	}
	else if (this->statuses[1] == EntityStatus::ARMS_MOVING_AROUND_RUNNING)
	{
		for (int i = 0; i < this->statuses.size(); ++i)
			SpriteRenderer::get().draw(ResourceManager::getShader("sprite"), ResourceManager::getFlipbook(this->animationsName2D[this->statuses[i]]).getTextureAtTime(GlobalClock::get().getCurrentTime() - this->timesSinceStatuses[i]), Camera::get().screenPosition(this->x, this->y), Camera::get().screenSize(this->drawWidth + this->runningOffsetSize * glm::sin(this->runningOffsetSpeed * (GlobalClock::get().getCurrentTime() - this->timesSinceStatuses[i])), this->drawHeight + this->runningOffsetSize * glm::sin(this->runningOffsetSpeed * (GlobalClock::get().getCurrentTime() - this->timesSinceStatuses[i]))), this->rotateAngle);
	}
	else
	{
		for (int i = 0; i < this->statuses.size(); ++i)
			SpriteRenderer::get().draw(ResourceManager::getShader("sprite"), ResourceManager::getFlipbook(this->animationsName2D[this->statuses[i]]).getTextureAtTime(GlobalClock::get().getCurrentTime() - this->timesSinceStatuses[i]), Camera::get().screenPosition(this->x, this->y), Camera::get().screenSize(this->drawWidth, this->drawHeight), this->rotateAngle);
	}
}

void Player::modifyBullets(Weapon::WeaponType weaponType, int amount)
{
	bullets[weaponType] += amount;
}

void Player::save()
{
	std::ofstream saveFile("config/save.json");
	nlohmann::json saveJSON;

	saveJSON["health"] = health;
	saveJSON["healthCap"] = healthCap;

	saveJSON["armor"] = armor;
	saveJSON["armorCap"] = armorCap;

	saveJSON["staminaCap"] = staminaCap;

	saveJSON["gold"] = gold;

	// TODO

	saveFile << std::setw(4) << saveJSON << std::endl;
	saveFile.close();
}

void Player::load()
{
	std::ifstream saveFile("config/save.json");
	nlohmann::json saveJSON;
	saveFile >> saveJSON;
	saveFile.close();

	health = saveJSON["health"].get<double>();
	healthCap = saveJSON["healthCap"].get<double>();

	armor = saveJSON["armor"].get<double>();
	armorCap = saveJSON["armorCap"].get<double>();

	staminaCap = saveJSON["staminaCap"].get<double>();

	gold = saveJSON["gold"].get<int>();

	// TODO
}


void Player::enterShop()
{
	MenuManager::get().push(ShopMenuWeapons::get());
	InputHandler::setInputComponent(InputHandler::getMenuInputComponent());
}

