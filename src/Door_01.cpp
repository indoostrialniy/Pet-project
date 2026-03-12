#include "Door_01.h"

#include "macros.h"
#include "CollisionDescriptor.h"

#include "ElectricBox.h"
#include "Level.h"


Door_01::Door_01( glm::vec3 worldLoc, glm::quat worldRot, std::string n )
{
	MicroWorldName = n;
	
	position = worldLoc;		//назначим координаты
	rotation = worldRot;

	//~ addEntity( "Doorway", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Doorway_01", { 3, 2.0f, "CB_Doorway_01"  }, "MESH", { "M_Default_05k",  "M_Default_05k", },  "none",  "forward" );
	//~ addEntity( "Door", glm::vec3(0.0609, -0.5863, 1.6546), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Door_01", { 3, 2.0f, "CB_Door_01" }, "MESH", { "M_Default_05k",  "M_Default_05k",  "M_Default_05k", },  "none",  "forward" );

	CollisionDescriptor DoorWayCollision;
		DoorWayCollision.collisionType = 3;
		DoorWayCollision.collisionRadius = 2.0f;
		DoorWayCollision.complexCollisionFileName = "CB_Doorway_01";
		
	std::vector<std::string> doorWayMaterials = { "M_Concrete_007_0.5k_x2",  "M_Dark_Wood_0_5k_x1" };
	std::vector<std::string> doorMaterials = { "M_Metal_Rust_002_0_5k_x1",  "M_Default_0_5k",  "M_Metal_Rust_001_0_5k_x1"  };

		
	CollisionDescriptor DoorCollision;
		DoorCollision.collisionType = 3;
		DoorCollision.collisionRadius = 2.0f;
		DoorCollision.complexCollisionFileName = "CB_Door_01";

	addEntity( "Doorway", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Doorway_01", DoorWayCollision, "MESH", doorWayMaterials,  "none",  "forward" );
	addEntity( "Door", glm::vec3(0.0609, -0.5863, 1.6546), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Door_01", DoorCollision, "MESH", doorMaterials,  "none",  "forward" );


	treshold = 2;

	// так как родительский класс - MovingPartsController, то для него указываем, какая сущность каким компонентом является и уточняем целевой кватернион
	lidProcConfig doorMovingPart;
		doorMovingPart.name = "Door";
		doorMovingPart.keyCode = GLFW_KEY_E;							// реагирует на клавишу Е
		doorMovingPart.transformationType = MMP_ROTATABLE;				// вращается
		doorMovingPart.targetQuat = glm::quat(0.383, 0,0,-0.924);		//
		doorMovingPart.timeForRotation = 1500;	
		doorMovingPart.start_callback = std::bind(&Door_01::opening_start, this);	
		doorMovingPart.end_callback = std::bind(&Door_01::opening_end, this);
		doorMovingPart.powValue = 3;
		
	mainMovingPartsMassive[MMP_ZERO]  = std::move(doorMovingPart);	
	
	//lidProcConfig("Door", false, MMP_ROTATABLE,  glm::quat(1.0, 0.0, 0.0, 0.0), glm::quat(0.383, 0,0,-0.924), 1500, 3);

	//~ movePartBuffer.loadFromFile("./datas/audio/zvuk_skripa_bolshih_vorot_kotorie_pitayutsya_otkryt.wav");
	//~ movePartSound = sf::Sound(movePartBuffer);
	status = true;
}


void Door_01::logic() 	// типовая логика из класса MovingPartsController будет заменена методом из этого класса
{					
	//~ 	//пробуем получить умный указатель на экземпляр камеры игрока
	
	//~ if(player) // если получили, то идем дальше
	//~ {
		//~ bool playerArmed =  player->childMicroWorld != nullptr ? true : false;	// если у игрока есть привязанный дочерний объект, то
		
		//~ processMMP( MMP_ZERO, playerArmed );
	//~ }	
	
	auto chk = [&]()	{ 	if( CHECK_PTR(CameraPositionPtr) ) 		{	float length = glm::length(*CameraPositionPtr-position);		return  length < treshold ? true : false;		} 
							else 			{	printMessage("Camera ptr is invalid")			return true;	} 		};
	
	
	
	if( MicroWorldName == "Door_04" && glfwGetKey(engineWindow, GLFW_KEY_E) && chk() )	// дверь наружу
	{
		printMessage("\n\n\nGAME OVER, nothing more yet")
		//~ glfwSetWindowShouldClose(engineWindow, GLFW_TRUE);
	}
	
	
	if( MicroWorldName == "Door_02")	// дверь в оружейку
	{
		if(glfwGetKey(engineWindow, GLFW_KEY_E) && !doorOpeningOpinion && chk() )
		{
			doorOpeningOpinion = true;
			
			sound_doorLocked->playSound( std::bind(&Door_01::lockedSound_start, this), std::bind(&Door_01::lockedSound_end, this) );
			
			sound_doorLocked2->playSound( [](){}, [](){} );
		}
		
		if(glfwGetKey(engineWindow, GLFW_KEY_E) && electicaOn && !door_triggered)
		{
			door_triggered = true;
			
			launchMainMovingPartMovement(MMP_ZERO, std::bind(&Door_01::opening_start, this), std::bind(&Door_01::opening_end, this));
		}
	}
	
	//if( MicroWorldName == "Door_02") // дверь из оружейки
	else
	{
		if( glfwGetKey(engineWindow, GLFW_KEY_E) && !door2_3_opened && chk() && !locked)
		{
			door2_3_opened = true;	// просто вспомоательнй флаг
			launchMainMovingPartMovement(MMP_ZERO, std::bind(&Door_01::opening_start, this), std::bind(&Door_01::opening_end, this));
			
			opened = !opened;
		}
		
		if( !glfwGetKey(engineWindow, GLFW_KEY_E) )
		{
			door2_3_opened = false;
		}
	}
	
		
}

void Door_01::lockedSound_start()
{
	
}

void Door_01::lockedSound_end()	// только после воспроизведения мысли о питании
{
	std::shared_ptr<Electric_Box> elbox = dynamic_pointer_cast<Electric_Box>( level->getActor("Electric_Box_01") );
	if(elbox)
	{
		elbox->electricBoxActive = true;
	}
}


void Door_01::opening_start()
{
	sound_doorOpening->playSound( [](){}, [](){} );
}

void Door_01::opening_end()
{
	sound_doorOpening->stopSound();
}
