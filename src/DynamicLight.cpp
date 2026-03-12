#include "DynamicLight.h"


DynamicLight_01::DynamicLight_01( glm::vec3 worldLoc, glm::quat worldRot, std::string n )
{
	MicroWorldName = n;
	
	position = worldLoc;		//назначим координаты
	rotation = worldRot;
	
	
	addTag("Light");
	
	visible = false;
	
	status = true;	// у неосязаемого комбо не должно быть проблем с инициализацией компонентов
}
