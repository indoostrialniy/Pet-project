#include "Box_01.h"
#include "CollisionDescriptor.h"
#include "SoundAsset.h"

Box_01::Box_01( glm::vec3 worldLoc, glm::quat worldRot, std::string n )
{
	MicroWorldName = n;
	
	position = worldLoc;		//назначим координаты
	rotation = worldRot;

	CollisionDescriptor lidCollision;
		lidCollision.collisionRadius = 1.0f;
		lidCollision.collisionType = 3;
		lidCollision.complexCollisionFileName = "CB_Box_01_Lid";
	
	CollisionDescriptor boxCollision;
		boxCollision.collisionRadius = 1.0f;
		boxCollision.collisionType = 3;
		boxCollision.complexCollisionFileName = "CB_Box_01_Box";

	addEntity( "SM_Box_01_Lid", glm::vec3(-0.2001, -0.0, 0.327), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Armybox_01_Lid", lidCollision, "MESH", { "M_WoodPlanks_0_5k", },  "none",  "forward" );
	addEntity( "SM_Box_01_Box", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Armybox_01_Box", boxCollision, "MESH", { "M_WoodPlanks_0_5k", },  "none",  "forward" );

	treshold = 2.0f;

	movePartSoundPtr = 	SoundAssets.getAsset("zvuk_rgavoy_metallicheskoy_dveri.wav");

	lidProcConfig lidMovingPart;										// создадим структуру с конфигурацией двигающегося компонента
		lidMovingPart.name = "SM_Box_01_Lid";							// указываем конкретны компонент
		lidMovingPart.keyCode = GLFW_KEY_E;								// реагирует на клавишу T
		lidMovingPart.transformationType = MMP_ROTATABLE;				// вращается
		
		lidMovingPart.targetQuat = glm::quat(0.383, 0,-0.924, 0);		//
		lidMovingPart.timeForRotation = 1000;
			
		lidMovingPart.start_callback = [&](){ movePartSoundPtr->playSound( [](){}, [](){} ); };
		lidMovingPart.end_callback	 = [&](){ movePartSoundPtr->stopSound(); };	
					
	mainMovingPartsMassive[MMP_ZERO]  = std::move(lidMovingPart); 		// переместили в массив конфигураций компонентов
	
	status = components.size() == 2;
}

// XXX в методах можно перечислять все нужные скриптовки, а именно необходимую конкретному экземпляру указывать извне


//~ void Box_01::test_callback()
//~ {
	//~ printVarComment("test callback script for: ", MicroWorldName)
//~ }

// эти методы должны удовлетворять требованиям секвенсора - принимать кастомный тип BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE аргумента и возвращать буль




//~ void Box_01::logic() 	// типовая логика из класса MovingPartsController будет заменена методом из этого класса
//~ {
	//~ auto chk = [&]()	{ 	if( CHECK_PTR(CameraPositionPtr) ) 		{	float length = glm::length(*CameraPositionPtr - position);		return  length < treshold ? true : false;		} 
							//~ else 			{	printMessage("Camera ptr is invalid")			return true;	} 		};
							
	
	// processMMP(GLFW_KEY_P, flag_E, MMP_ZERO, chk() );
							
	//~ if(glfwGetKey(engineWindow, GLFW_KEY_E) && !triggered && chk() )
	//~ {
		//~ printMessage("try to logic: ")
		
		//~ triggered = true;
		
		//~ launchMainMovingPartMovement( MMP_ZERO, std::bind(&Box_01::soundOn, this), std::bind(&Box_01::soundOff, this) );	
	//~ }
	
	//~ if( !glfwGetKey(engineWindow, GLFW_KEY_P) )
	//~ {
		//~ triggered = false;
	//~ }
	
//~ }


//~ void Box_01::soundOn()
//~ {
	//~ movePartSoundPtr->playSound();
//~ }


//~ void Box_01::soundOff()
//~ {
	//~ movePartSoundPtr->stopSound();
	
	//~ lootingSound->playSound( std::bind(&Box_01::lootingSound_start, this), std::bind(&Box_01::lootingSound_end, this) );
//~ }


//~ void Box_01::lootingSound_start()
//~ {
	
//~ }
//~ void Box_01::lootingSound_end()
//~ {
	//~ auto pistol = dynamic_pointer_cast<Pistol>(level->getActor("Pistol_01"));
	//~ if(pistol)
	//~ {
		//~ pistol->canBeTaken = true;
	//~ }
//~ }

















//~ bool Box_01::box_open_script(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
//~ {
	//~ printVarComment("Open script for: ", MicroWorldName)
	//~ movePartSoundPtr->playSound();
	
	//~ return true;
//~ }



//~ bool Box_01::box_close_script(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
//~ {
	//~ printVarComment("Close script for: ", MicroWorldName)
	//~ movePartSoundPtr->stopSound();	
	//~ return true;
//~ }


//~ bool Box_01::box_open_script_2(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
//~ {
	//~ printVarComment("box_open_script_2 for: ", MicroWorldName)
	//~ movePartSoundPtr->playSound();
	
	//~ return true;
//~ }

