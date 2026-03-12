#include "Debugger.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "CollisionDescriptor.h"

Debugger::Debugger( std::string name )
{
	MicroWorldName = name;
	
	position = glm::vec3(0,0,0);		//назначим координаты
	rotation = glm::quat(1,0,0,0);
	
	CollisionDescriptor MarkerCollision;
		MarkerCollision.collisionType = NO_COLLISION;


	addEntity( "Mark_00", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_01", },  "none",  "forward" );
	addEntity( "Mark_01", glm::vec3(0.0, 0.0, 1.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_01", },  "none",  "forward" );
	addEntity( "Mark_02", glm::vec3(0.0, 0.0, 2.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_01", },  "none",  "forward" );
	
	addEntity( "Mark_03", glm::vec3(1.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_02", },  "none",  "forward" );
	addEntity( "Mark_04", glm::vec3(1.0, 0.0, 1.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_02", },  "none",  "forward" );
	addEntity( "Mark_05", glm::vec3(1.0, 0.0, 2.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_02", },  "none",  "forward" );
	
	addEntity( "Mark_06", glm::vec3(0.0, 1.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_03", },  "none",  "forward" );
	addEntity( "Mark_07", glm::vec3(0.0, 1.0, 1.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_03", },  "none",  "forward" );
	addEntity( "Mark_08", glm::vec3(0.0, 1.0, 2.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_03", },  "none",  "forward" );
	
	addEntity( "Mark_09", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_04", },  "none",  "forward" );
	addEntity( "Mark_10", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_04", },  "none",  "forward" );
	addEntity( "Mark_11", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_04", },  "none",  "forward" );
	addEntity( "Mark_12", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_02", },  "none",  "forward" );
	addEntity( "Mark_13", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_02", },  "none",  "forward" );
	addEntity( "Mark_14", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_02", },  "none",  "forward" );

}
	
