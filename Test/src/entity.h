#ifndef ENTITY_H__
#define ENTITY_H__

#include <vector>
#include "define.h"
#include "iComponent.h"
#include "physicsComponent.h"
#include "playerPhysicsComponent.h"
#include "graphicsComponent.h"
#include "inputComponent.h"
#include "playerInputComponent.h"
#include "playerGraphicsComponent.h"
#include "audioComponent.h"
#include "renderer.h"

class Entity
{
public:
	Entity(unsigned int id, bool playerInput = false);
	~Entity();
	void SetRender();
	void Update();
	void Draw();
	unsigned int GetID();
	std::shared_ptr<PhysicsComponent> Physics() { return m_physicsComponent; }
	std::shared_ptr<GraphicsComponent> Graphics() { return m_graphicsComponent; }
	std::shared_ptr<InputComponent> Input() { return m_inputComponent; }
	std::vector<std::shared_ptr<IComponent>> components;
	void Communicate(std::vector<std::string> msg);
private:
	unsigned int m_id;
	std::shared_ptr<PhysicsComponent> m_physicsComponent;
	std::shared_ptr<GraphicsComponent> m_graphicsComponent;
	std::shared_ptr<InputComponent> m_inputComponent;
};

#endif