#include "InteractionManager.h"

#include <glm/vec2.hpp>

#include "../Entity/InteractiveEntity.h"
#include "../Entity/Door/Door.h"

#include <iostream> // TODO: delete
#include <memory>

InteractionManager::InteractionManager()
{

}

InteractionManager::~InteractionManager()
{

}

InteractionManager& InteractionManager::get()
{
	static InteractionManager instance;

	return instance;
}

void InteractionManager::handleInteractions(std::vector<std::shared_ptr<Entity>>& entities)
{
	// TODO: implementare

	// Player vs. Entities
	// momentan doar door
	for (int i = 0; i < entities.size(); ++i)
	{
		if (std::dynamic_pointer_cast<InteractiveEntity>(entities[i]) == nullptr)
			continue;

		if (std::dynamic_pointer_cast<InteractiveEntity>(entities[i])->isInInteraction())
			std::dynamic_pointer_cast<InteractiveEntity>(entities[i])->onInteraction();
	}
}

