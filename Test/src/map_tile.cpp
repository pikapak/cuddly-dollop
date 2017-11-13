#include "map_tile.h"

MapTile::MapTile(Vector3f pos, std::string modelName, std::string texPath, Vector3f bbsize, Vector3f center)
	: m_phys(std::shared_ptr<PhysicsComponent>(new PhysicsComponent(pos, modelName, bbsize, center))), m_modelName(modelName), m_texPath(texPath)
{
	m_trans = Transformation();
	m_trans.SetTranslation(pos);
}