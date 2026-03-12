#include "Gate_01.h"
#include "Orc.h"

#include "CollisionDescriptor.h"

#include "macros.h"
#include "Level.h"
#include "Entity.h"
#include "WallBreach.h"


Gate_01::Gate_01( glm::vec3 worldLoc, glm::quat worldRot, std::string n )
{
	MicroWorldName = n;
	
	position = worldLoc;		//назначим координаты
	rotation = worldRot;

	CollisionDescriptor emptyCollision;

	std::vector<std::string> gateMats = { "M_Concrete_007_0.5k_x2",  "M_Metal_Rust_002_0_5k_x1" };
	std::vector<std::string> gateDoorRMats = { "M_Metal_Rust_001_0_5k_x4",  "M_Metal_Rust_002_0_5k_x1" };
	std::vector<std::string> gateDoorLMats = { "M_Metal_Rust_001_0_5k_x4",  "M_Metal_Rust_002_0_5k_x1" };
	std::vector<std::string> gateWheelMats = { "M_Metal_Rust_001_0_5k_x4",  "M_Concrete_007_0.5k_x2",  "M_Metal_Rust_002_0_5k_x1" };


	bool ent1 = addEntity( "Gate_01", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Massivegate_01", emptyCollision, "MESH", gateMats,  "none",  "forward" );
	bool ent2 = addEntity( "SM_Gate_01_Door_Right", glm::vec3(0.1923, -1.7725, 1.4658), glm::quat(1.0,0.0,0.0,0.0),  "SM_Gate_01_Door_Right", emptyCollision, "MESH", gateDoorRMats,  "none",  "forward" );
	bool ent3 = addEntity( "SM_Gate_01_Door_Left", glm::vec3(0.193, 1.7706, 1.4712), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Gate_01_Door_Left", emptyCollision, "MESH", gateDoorLMats,  "none",  "forward" );
	bool ent4 = addEntity( "SM_Gate_01_Wheel", glm::vec3(0.0482, 0.3313, 0.8922), glm::quat(1.0,0.0,0.0,0.0),  "SM_Gate_01_Wheel", emptyCollision, "MESH", gateWheelMats,  "none",  "forward" );

	treshold = 3.0;

	// так как родительский класс - MovingPartsController, то для него указываем, какая сущность каким компонентом является и уточняем целевой кватернион
	lidProcConfig wheelMovingPart;
		wheelMovingPart.name = "SM_Gate_01_Wheel";
		wheelMovingPart.keyCode = GLFW_KEY_E;							// реагирует на клавишу Е
		wheelMovingPart.transformationType = MMP_ROTATABLE;				// вращается
		wheelMovingPart.targetQuat = glm::quat(0, -1, 0, 0);		//
		wheelMovingPart.timeForRotation = 7000;	
		wheelMovingPart.powValue = 3;
		
	mainMovingPartsMassive[MMP_ZERO]  = std::move(wheelMovingPart);	
	
	
	lidProcConfig doorMovingPart;
		doorMovingPart.name = "SM_Gate_01_Door_Right";
		doorMovingPart.keyCode = GLFW_KEY_R;							// реагирует на клавишу R
		doorMovingPart.transformationType = MMP_ROTATABLE;				// вращается
		doorMovingPart.targetQuat = glm::quat(0.991, 0, 0, -0.131);		//
		doorMovingPart.timeForRotation = 7000;	
		doorMovingPart.powValue = 3;
		
	mainMovingPartsMassive[MMP_FIRST]  = std::move(doorMovingPart);	
	
	
	status = (ent1 && ent2 && ent3 && ent4);
}


void Gate_01::logic() 	// типовая логика из класса MovingPartsController будет заменена методом из этого класса
{				
	
	//~ auto player = level->getActor("Camera");
	
	//~ if(player)
	//~ {
		//~ glm::quat rot = player->getQuaternionFocusedToPoint( components["SM_Gate_01_Wheel"]->getWorldPosition() );
		//~ printQuat(rot)
	//~ }
		
		
	auto chk = [&]()	{ 	if( CHECK_PTR(CameraPositionPtr) ) 		{	float length = glm::length(*CameraPositionPtr - position);		return  length < treshold ? true : false;		} 
						else 			{	printMessage("Camera ptr is invalid")			return true;	} 		};
			
	
	if( glfwGetKey(engineWindow, GLFW_KEY_E) && chk() && !bGateActivated ) // если получили, то идем дальше
	{
		bGateActivated = true;
				
		auto player = level->getActor("Camera");
		
		if(player)
		{
			// запуск цикла фокусировки этого комбо на указанную точку и явное указание безаргументных колбеков
			player->focuseToSpecifiedPoint( components["SM_Gate_01_Wheel"]->getWorldPosition(), 2000, 		//PointWorldSpacePosition(), 2000,
											std::bind(&Gate_01::approachingForOpeningGate_start, this),
											std::bind(&Gate_01::approachingForOpeningGate_end, this)	);	
		}
		
	}		

	//~ if(!glfwGetKey(engineWindow, GLFW_KEY_E) )
	//~ {
		///bWallBreachInitialized = false;
		//~ bGateActivated = false;
	//~ }

}

void Gate_01::approachingForOpeningGate_start( )
{
	//~ printVarComment("approachingToopeningGate_end clb for: ", MicroWorldName)
}

void Gate_01::approachingForOpeningGate_end( )
{
	//~ printVarComment("approachingToopeningGate_end clb for: ", MicroWorldName)
	
	// запуск последовательности движения компонента и явное назначение ему конкретных колбеков
	launchMainMovingPartMovement( 	MMP_ZERO, [](){}, [](){} );							// штурвал, std::bind(&Gate_01::openingGate_start, this), std::bind(&Gate_01::openingGate_end, this) );
	launchMainMovingPartMovement( 	MMP_FIRST, std::bind(&Gate_01::openingGate_start, this), std::bind(&Gate_01::openingGate_end, this) );	// створка гермоворот
}



void Gate_01::openingGate_start()
{
	//~ printMessage("Start opening gate")
	
	gateOpenSoundPtr->playSound([](){}, [](){});
	whellTurningSoundPtr->playSound([](){}, [](){});
}

void Gate_01::openingGate_end()
{
	//~ printMessage("End opening gate")
	
	gateOpenSoundPtr->stopSound();
	whellTurningSoundPtr->stopSound();
	gateFailureSoundPtr->playSound( std::bind(&Gate_01::gateFailureSound_start, this), std::bind(&Gate_01::gateFailureSound_end, this) );
	
	
	auto orc = dynamic_pointer_cast<Orc>(level->getActor("Orc_01"));
	if(orc)
	{
		orc->spotted = true;
	}
	
	//~ auto orc2 = dynamic_pointer_cast<Orc>(level->getActor("Orc_02"));
	//~ if(orc2)
	//~ {
		//~ orc2->spotted = true;
	//~ }
	
	//~ auto orc3 = dynamic_pointer_cast<Orc>(level->getActor("Orc_03"));
	//~ if(orc3)
	//~ {
		//~ orc3->spotted = true;
	//~ }
}

void Gate_01::gateFailureSound_start()
{
	
}

void Gate_01::gateFailureSound_end()
{
	std::shared_ptr<WallBreach_01> wallbreach = std::dynamic_pointer_cast<WallBreach_01>( level->getActor("WallBreach_01") );
	if(wallbreach)
	{
		wallbreach->bWallBreachInitialized = true;
	}
}

////////////////////////////////

//~ bool Gate_01::approachingForOpeningGate_start(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
//~ {
	//~ printVarComment("approachingToopeningGate_end for: ", MicroWorldName)
	
	//~ return true;
//~ }

//~ bool Gate_01::approachingForOpeningGate_end(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
//~ {
	//~ printVarComment("approachingToopeningGate_end for: ", MicroWorldName)
	
	//~ return true;
//~ }


//~ BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;	
//~ datas.var = MMP_FIRST;
//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function 	= std::bind( &MovingPartsController::rotateUniversal, this, datas);	// главная вращающая функция
//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> start 		= std::bind( &Gate_01::openingGate_start, this, datas);		// колбек начала
//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> end 		= std::bind( &Gate_01::openingGate_end, this, datas);		// колбек конца
//~ Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
//~ datas.var = MMP_ZERO;
//~ function 	= std::bind( &MovingPartsController::rotateUniversal, this, datas);	// главная вращающая функция
//~ Flow.CallSequence( { std::move(function), std::move(datas) } );


// назначим позицию для вращения штурвала гермодвери
//~ BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;
	//~ datas.focuseDuration = 2000;
	//~ datas.currentPoint = player->getPosition();
	//~ datas.currentRot = player->getRotation();
	//~ datas.focusedPoint = components["Gate_01"]->getPointWorldSpacePosition( glm::vec3(1.0482, 0.3313, 1.3922) );
	//~ datas.focusedRot = player->getQuaternionFocusedToPoint( components["SM_Gate_01_Wheel"]->getWorldPosition() ); //glm::quat(1,0,0,0);
//~ // запустим приближение				
//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function 	= std::bind( &MicroWorld::focuse, 	player, datas);	// фокусирующая функция
//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> start 		= std::bind( &Gate_01::approachingForOpeningGate_start, this, datas); 
//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> end 		= std::bind( &Gate_01::approachingForOpeningGate_end, this, datas) ; 
//~ Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
