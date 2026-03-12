#include "Dynamic_Ball.h"


Dynamic_Ball::Dynamic_Ball( glm::vec3 worldLoc, glm::quat worldRot, std::string n="Ball_noname" )
{
	MicroWorldName = n;
	
	position = worldLoc;// glm::vec3(0,0,10);
	rotation = worldRot;//glm::quat(1,0,0,0);
	
	startupPosition = worldLoc;
	startupRotation = worldRot;
	
	
	///reset();
	
	
	CollisionDescriptor ICO;
	ICO.collisionRadius = 0.5f;
	ICO.collisionType = SPHERE_COLLISION;
	
	addEntity( "Icosphere", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "Icosphere.001", ICO, "MESH", { "M_Default_1k", },  "none",  "forward" );


	glm::vec3 moveDirection = glm::vec3(-0.707, 0,-0.707);		// вектор текущего перемещения комбо

}


void Dynamic_Ball::update()						// обновление состояний сета вызывается из функции MicroWorld::MicroWorldUpdate(). Может быть переопределено дочерне
{
	
	if( glfwGetKey(engineWindow, GLFW_KEY_R) )
	{
		reset();
		
		//~ position = glm::vec3(0.37, 6, 15);
		moveDirection = glm::vec3(0,0,-1);
	}
	
	
	// начинаем падение с высоты 15 метров без начального ускорения
	
	
	//~ t += tick;
	
	//~ if(glfwGetKey(engineWindow, GLFW_KEY_J))
	//~ {
		//~ t = 0;
		//~ position = glm::vec3(-9,0,15);
		//~ speed = 0.0f;
	//~ }
	
	
	//~ // XXX гравитационный функционал этой функции можно в дальнейшем перенести в отдельный класс Gravity	
	//speed += 9.8f / 60.0f/4.0f/100.0f * float(tick)/1000.0f ;
	speed = 9.8f*float(tick)/1000.0f; 
	
	
	//~ if( glm::length(position - glm::vec3(3.70672, 0, 0.564) ) > 0.1f )
	{
		//~ printVariable(speed) // 0.01568
	
		
		static bool f1 = false;
		static bool f2 = false;
		static bool f3 = false;
		static bool f4 = false;
		static bool f5 = false;
		static bool f6 = false;
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_8) && !f1) { moveInDirection( "Icosphere", glm::vec3(1,0,0), speed ); 
			}
			//~ f1 = true;}
		//~ if( !glfwGetKey(engineWindow, GLFW_KEY_KP_8) ) {f1 = false;}
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_2) && !f2) { moveInDirection( "Icosphere", glm::vec3(-1,0,0), speed );
			}
			//~ f2 = true;}
		//~ if( !glfwGetKey(engineWindow, GLFW_KEY_KP_2) ) {f2 = false;}
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_4) && !f3) { moveInDirection( "Icosphere", glm::vec3(0,1,0), speed );
			}
			//~ f3 = true;}
		//~ if( !glfwGetKey(engineWindow, GLFW_KEY_KP_4) ) {f3 = false;}
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_6) && !f4) { moveInDirection( "Icosphere", glm::vec3(0,-1,0), speed ); 
			}
			//~ f4 = true;}
		//~ if( !glfwGetKey(engineWindow, GLFW_KEY_KP_6) ) {f4 = false;}
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_9) && !f5) { moveInDirection( "Icosphere", glm::vec3(0,0,1), speed ); 
			}
			//~ f5 = true;}
		//~ if( !glfwGetKey(engineWindow, GLFW_KEY_KP_9) ) {f5 = false;}
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_7) && !f6) { moveInDirection( "Icosphere", glm::vec3(0,0,-1), speed ); 
			}
			//~ f6 = true;}
		//~ if( !glfwGetKey(engineWindow, GLFW_KEY_KP_7) ) {f6 = false;}
		
	}
	
	//~ float potentialEnergy = mass * g * 12.7f;
	//~ printVariable(potentialEnergy)

	
	
	//~ float kineticEnergy = mass * speed*speed/2.0f * 100.0f * 60.0f * 4.0f / (float(tick)/1000.0f) ;
	//~ printVariable(kineticEnergy)
	//~ static glm::vec3 gravity = glm::vec3(0,0,0);
	//~ runtimePositionChange(gravity);
	
	//moveDirection -= glm::vec3( 0, 0, float(t)/1000.0f*g );
	
	
	
	
	//~ if( glfwGetKey(engineWindow, GLFW_KEY_J) )
	//~ {
		//~ position += moveDirection * speed;
		//~ bool cast = bCheckIntersections( "Icosphere", position, moveDirection, v3Reflect, fSlideCoeff  );	// проверяем, не стали ли пересекать чью-либо коллизию	
		//~ if(cast) // если пересекли кого-то
		//~ {
			//~ position -= moveDirection * speed;	//9.8f*tick/1000.0f; 				//glm::vec3(0,0,1) * fMoveSpeedZ*offfdfmsds.z; //сделаем такой же шаг назад
			//~ moveDirection = v3Reflect;	// при столкновении перенаправляемся по вектору отражения
		//~ }
	//~ }
	if( glfwGetKey(engineWindow, GLFW_KEY_J) )
	{
		printMessage("\nBall update")
		
		///moveDirection += glm::vec3(0,0,-0.01);
		
		moveInDirection( "Icosphere", moveDirection, speed );
	}
		
}


