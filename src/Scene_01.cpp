#include "Scene_01.h"
#include "CollisionDescriptor.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

Scene_01::Scene_01( std::string name )
{
	MicroWorldName = name;
	
	CollisionDescriptor SceneCollision;
		SceneCollision.collisionType = COMPLEX_COLLISION;
		SceneCollision.collisionRadius = 100.0f;
		SceneCollision.complexCollisionFileName = "CB_Scene_01";
	
	bool ent1 = addEntity( "CB_Scene_01", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "CB_Scene_01", SceneCollision, "MESH", { "M_Default_0_5k", },  "none",  "forward" );
	
	setEntityVisibility( "CB_Scene_01", false);

	status = ent1;

}


void Scene_01::update()
{
	
	if(glfwGetKey(engineWindow, GLFW_KEY_TAB) && !changeCollisionVisibility) 
	{
		changeCollisionVisibility = true;
		
		setEntityVisibility( "CB_Scene_01", !getEntityVisibility("CB_Scene_01") );		// bool visibility);
	}
	
	if( !glfwGetKey(engineWindow, GLFW_KEY_TAB)) 
	{ 
		changeCollisionVisibility = false;	
	}		
}
