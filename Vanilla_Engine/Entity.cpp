
#include "Entity.h"
#include "ApplicationCore.h"

Entity::Entity() {

	id = Entity::GenerateEntityID();
}

Entity::~Entity() {


}

void Entity::Shutdown() {

	// Release all components connected to this entity
	components.Shutdown();
}




