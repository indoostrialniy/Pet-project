#include "Elevator.h"

#include "CollisionDescriptor.h"
#include "macros.h"

#include "Level.h"
#include "Entity.h"
#include "SoundAsset.h"
#include "Skeleton.h"
#include "SkeletonAnimation.h"

Elevator::Elevator( glm::vec3 worldLoc, glm::quat worldRot, std::string n)
{
	MicroWorldName = n;
	
	position = worldLoc;		//назначим координаты
	rotation = worldRot;

	//~ printMessage("Elevator constructor")

	CollisionDescriptor DoorCollision;
		DoorCollision.collisionType = COMPLEX_COLLISION;
		DoorCollision.collisionRadius = 3.55f;
		DoorCollision.complexCollisionFileName = "CB_Elevator_01_Cabin_Door";
	addEntity( "Door", glm::vec3(1.04, -0.44, 0.1), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Elevator_01_Cabin_Door", DoorCollision, "MESH", { "M_Metal_Rust_002_0_5k_x1",  "M_Dark_Wood_0_5k_x1", },  "none",  "forward" );
	addEntity( "Door_Left", glm::vec3(1.04, 0.44, 0.1), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Elevator_01_Cabin_Door", DoorCollision, "MESH", { "M_Metal_Rust_002_0_5k_x1",  "M_Dark_Wood_0_5k_x1", },  "none",  "forward" );
	
	//~ setEntityVisibility( "Door", true);
	//~ setEntityVisibility( "Door_Left", true);
	
	CollisionDescriptor ElevatorCollision;
		ElevatorCollision.collisionType = COMPLEX_COLLISION;
		ElevatorCollision.collisionRadius = 3.55f;
		ElevatorCollision.complexCollisionFileName = "CB_Elevator_01_Cabin";
	addEntity( "Cabin", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Elevator_01_Cabin", ElevatorCollision, "MESH", { "M_Dark_Wood_0_5k_x1",  "M_Metal_Rust_002_0_5k_x4", },  "none",  "forward" );
		
	CollisionDescriptor lampCollision;
		lampCollision.collisionType = COMPLEX_COLLISION;
		lampCollision.collisionRadius = 3.55f;
		lampCollision.complexCollisionFileName = "CB_Elevator_Lamp";
	addEntity( "SM_Elevator_Lamp", glm::vec3(-0.2368, 0.0405, 2.3183), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_elevator_Lamp", lampCollision, "MESH", { "M_Metal_Rust_002_0_5k_x1", },  "none",  "forward" );

	//~ setEntityVisibility( "Cabin", true);
	//~ setEntityVisibility( "SM_Elevator_Lamp", true);

	lampFallingSoundPtr = SoundAssets.getAsset("metallicheskaya-truba-padayuschiy-zvuk.wav");
	elevatorOpenIdeaSoundPtr = SoundAssets.getAsset("openElevatorIdea.wav");
		
		
	//~ addEntity("Skybox_01", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0), "SM_Skybox_01", {}, "MESH", { "M_Skybox_01", }, "none", "forward");
	
	// так как родительский класс - MovingPartsController, то для него указываем, какая сущность каким компонентом является и уточняем целевой кватернион
	
	treshold = 1.75; // порог по расстоянию до игрока, меньше которого срабатывают клавиши E, R
	
	
	lidProcConfig DoorRight;
		DoorRight.name = "Door";
		DoorRight.keyCode = GLFW_KEY_T;
		DoorRight.transformationType = MMP_MOVABLE;
		DoorRight.defaultPose = glm::vec3(1.04, -0.44, 0.1);
		DoorRight.targetPose = glm::vec3(1.04, -1.15, 0.1);	//glm::vec3(1.04, -1.24, 0.1);	
		DoorRight.timeForMoving = 1533;
	
	lidProcConfig DoorLeft;
		DoorLeft.name = "Door_Left";
		DoorLeft.keyCode = GLFW_KEY_Y;
		DoorLeft.transformationType = MMP_MOVABLE;
		DoorLeft.defaultPose = glm::vec3(1.04, 0.44, 0.1);
		DoorLeft.targetPose = glm::vec3(1.04, 1.15, 0.1);	//glm::vec3(1.04, 1.24, 0.1);
		DoorLeft.timeForMoving = 1533;
	
	
	lidProcConfig lamp;
		lamp.name = "SM_Elevator_Lamp";
		lamp.keyCode = GLFW_KEY_U;
		lamp.transformationType = MMP_MOVABLE;
		lamp.defaultPose = glm::vec3(-0.2368, 0.0405, 2.3183 );			//-0.186, 0.0, 2.28);
		
		lamp.targetPose = glm::vec3(-0.236817, 0.040506, 0.268762);	//glm::vec3(1.04, 1.24, 0.1);
		lamp.timeForMoving = 300;
		lamp.targetQuat = glm::quat(0.945831, -0.040901, 0.264592, 0.183636);
		lamp.timeForRotation = 200;
		//~ lamp.startFunction = std::bind(&Elevator::lampFallingSound, this, BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE() );
		//~ lamp.endFunction = [](BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE&){ std::cout<< "DEFAULT sdvsnvksksnvskvdsnvsvsk" << std::endl; }
	
	mainMovingPartsMassive[MMP_ZERO]  	= std::move(DoorRight);						//lidProcConfig("Door", 		false, MMP_MOVABLE, glm::vec3(1.04, -0.44, 0.1), glm::vec3(1.04, -1.24, 0.1), 1160);
	mainMovingPartsMassive[MMP_FIRST] 	= std::move(DoorLeft);						//lidProcConfig("Door_Left", 	false, MMP_MOVABLE, glm::vec3(1.04, 0.44, 0.1), glm::vec3(1.04, 1.24, 0.1), 1160);
	mainMovingPartsMassive[MMP_SECOND] 	= std::move(lamp);						//lidProcConfig("Door_Left", 	false, MMP_MOVABLE, glm::vec3(1.04, 0.44, 0.1), glm::vec3(1.04, 1.24, 0.1), 1160);



	movePartSoundPtr = std::make_unique<SoundAsset>( "zvuk_rgavoy_metallicheskoy_dveri.wav" ); 

	//~ movePartBuffer.loadFromFile("./datas/audio/zvuk_rgavoy_metallicheskoy_dveri.wav");
	//~ movePartSound = sf::Sound(movePartBuffer);

	status = components.size() == 4;

}




void Elevator::logic() 
{
	//~ // if press e and player is near
	//~ // лямбда функция, если копия камеры валидна, то смотреть разницу расстояния, иначе - тру
	auto chk = [&]()	{ 	if( CHECK_PTR(CameraPositionPtr) ) 		{	float length = glm::length(*CameraPositionPtr-(position+glm::vec3(0, 0, 2.3)));		return  length < treshold ? true : false;		} 
							else 			{	printMessage("Camera ptr is invalid")			return true;	} 		};
	
	auto player = level->getActor("Camera");
	// скрипт падения лампы
	//~ if(!bLampFalled)
	{
		
		if( player && glm::length(player->getPosition() - components["SM_Elevator_Lamp"]->getWorldPosition() ) < 1.58f && !jumpIdeaGiven )
		{	
			//~ printMessage("lamp fall called")
			jumpIdeaSoundPtr->playSound( [](){}, [](){} );
			//~ launchMainMovingPartMovement( &bLampFalled, MMP_SECOND);
			//launchMainMovingPartMovement( 	MMP_SECOND, std::bind( &Elevator::lampFalling_start, this), std::bind( &Elevator::lampFalling_end, this) );
			
			jumpIdeaGiven = true;
		}
		
		
		if( player && glm::length(player->getPosition() - components["SM_Elevator_Lamp"]->getWorldPosition() ) < 1.75f && jumpIdeaGiven && glfwGetKey(engineWindow, GLFW_KEY_SPACE) && !bLampFalled )
		{	
			//~ printMessage("lamp fall called")
			
			//~ launchMainMovingPartMovement( &bLampFalled, MMP_SECOND);
			launchMainMovingPartMovement( 	MMP_SECOND, std::bind( &Elevator::lampFalling_start, this), std::bind( &Elevator::lampFalling_end, this) );
			
			bLampFalled = true;
			
		}
	}
	
	
	
	//~ auto chk2 = [&]()	{ 	if( CHECK_PTR(CameraPositionPtr) ) 		{	float length = glm::length(*CameraPositionPtr-(position+glm::vec3(0, 0, 1.8)));		return  length < treshold ? true : false;		} 
							//~ else 			{	printMessage("Camera ptr is invalid")			return true;	} 		};
	
	if( player && bLampFalled && glm::length(player->getPosition() - components["Cabin"]->getWorldPosition() ) < 1.88f && !bPlayerInsideCabin )
	{
		elevatorOpenIdeaSoundPtr->playSound( [](){}, [](){} );
		bPlayerInsideCabin = true;
	}
	
	
	if( bPlayerInsideCabin && glfwGetKey(engineWindow, GLFW_KEY_E) && !bElevatorTriggered)
	{
		printMessage("OpenGates")
		bElevatorTriggered = true;
		
		//~ auto player = level->getActor("Camera");
		
		if(player && chk() )
		{
			player->goAndFocuseToSpecifiedPoint( components["Cabin"]->getPointWorldSpacePosition( glm::vec3(100, 0, 1.65) ), 
												 components["Cabin"]->getPointWorldSpacePosition( glm::vec3(0.6715, 0, 1.65) ),
												1000, std::bind(&Elevator::doorApproaching_start, this), std::bind(&Elevator::doorApproaching_end, this) );	
			
			
			
		}
		
	}
		
}


void Elevator::lampFalling_start()
{
	lampFallingSoundPtr->playSound( [](){}, [](){} );
}

void Elevator::lampFalling_end()
{
	//~ lampFallingSoundPtr->stopSound();
	components["SM_Elevator_Lamp"]->disableCollision();
}


void Elevator::doorApproaching_start()
{
	auto player = level->getActor("Camera");
	if(player)
	{
		player->overview = false;
	}
}
void Elevator::doorApproaching_end()
{
	BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;	
	std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function = std::bind( &Elevator::doorOpeningSequence, this, datas);	// главная вращающая функция
	auto start = std::bind( &Elevator::doorOpening_start, this);		// колбек начала
	auto end = std::bind( &Elevator::doorOpening_end, this);		// колбек конца
		
	Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
}


bool Elevator::doorOpeningSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
{
	int duration = 5000;
	auto camera = level->getActor("Camera");
	if(camera)
	{
		if(camera->components.count("Player_Hands_MESH") != 0)
		{
			duration = camera->components["Player_Hands_MESH"]->entitySkeletonPtr->sklAnimations["Hands_Knife_Open_Door"]->getDuration();	//.count( newAnim.animName )
		}
	}
	
	if(elevatorOpenDoorTimer > duration)
	{
		elevatorOpenDoorTimer = 0;
		return true;
	}
	
	
	if(elevatorOpenDoorTimer > 2900 && !elevatorDoorsOpened)
	{
		// start moving door
		//components["Dynamite"]->visible = true;
		launchMainMovingPartMovement(MMP_ZERO, [](){}, [](){} );
		launchMainMovingPartMovement(MMP_FIRST, [](){}, [](){} );
		
		elevatorOpenDoorSound->playSound( [](){}, [](){} );
		
		elevatorDoorsOpened = true;
	}
	
	elevatorOpenDoorTimer+= tick;
	
	return false;
	
}

void Elevator::doorOpening_start()
{
	auto player = level->getActor("Camera");
	
	if(player)
	{
		animStateConfig handsAnimation, noSecondAnimation;
			handsAnimation.mixCoeff = 1.0f;
			handsAnimation.mask = {};
			handsAnimation.animName = "Hands_Knife_Open_Door";
			handsAnimation.repeatCount = 1;
			handsAnimation.blendingTimeIn = 500;
		
		player->components["Player_Hands_MESH"]->setAnimation(0, handsAnimation, noSecondAnimation	);
	}
}

void Elevator::doorOpening_end()
{
	// spawn transporter?
	printMessage("Lift opened!")
	
	auto player = level->getActor("Camera");
	if(player)
	{
		player->overview = true;
	}
}






bool Elevator::focuse_end( BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data )
{
	printMessage("elevator doors focuse_end")
	
	
	
	auto player = level->getActor("Camera");
	
	if(player)
	{
		animStateConfig handsAnimation, noSecondAnimation;
			handsAnimation.mixCoeff = 1.0f;
			handsAnimation.mask = {};
			handsAnimation.animName = "Hands_Knife_Open_Door";
			handsAnimation.repeatCount = 1;
			handsAnimation.blendingTimeIn = 500;
		
		player->components["Player_Hands_MESH"]->setAnimation(0, handsAnimation, noSecondAnimation	);
	}
	
	
	return true;
}


//~ bool Elevator::goToGates(void* ptr, int var)
//~ {
	//~ IF_DYNAMIC_CAST(Elevator*, caster, ((Elevator*)ptr))
	//~ {
		//~ if( CHECK_PTR(CameraPositionPtr) && CHECK_PTR(CameraQuaternion) )
		//~ {
			
			//~ static std::shared_ptr<MicroWorld> cameraMicroWorld = level->getActor("Camera");
			
			//~ static glm::vec3 playerPos;
			//~ static glm::quat playerRot;
			
			//~ if(caster->goToGatesTimer < tick)
			//~ {
				//~ playerPos = *CameraPositionPtr;
				//~ playerRot = *CameraQuaternion;
				
				//~ cameraMicroWorld->overview = false;	//забираем управление
			//~ }	
			
			//~ //стартовые позиции для начала анимации открытия лифта
			//~ glm::vec3 elevatorOpenPositionStart = glm::vec3(-8.752, -0.245, 1.751);	// мировые координаты
			//~ glm::quat elevatorOpenRotationStart = glm::quat(1, 0, 0, 0);
			
			
			//~ caster->goToGatesTimer += tick;
	
			//~ float mix = (float)caster->goToGatesTimer/2500.0f;	// определим текущий коэффициет поворота
						
			//~ if(mix>1.01f)
			//~ {
				//~ caster->goToGatesTimer = 0;		
				
				//~ cameraMicroWorld->overview = true;	//возвращаем управление
				
				//~ //printMessage("\n\n ELEVATOR OPEN DOORS CALLED !\n")
				
				
				
				//~ Flow.CallSequence({"elevatorOpenDoor", caster->openGates, caster});								
				//~ return true;
			//~ }
			//~ else
			//~ {
				//~ *CameraPositionPtr = glm::mix( playerPos, elevatorOpenPositionStart, mix );
				//~ *CameraQuaternion = glm::slerp( playerRot, elevatorOpenRotationStart, mix  );	
				//~ return false;
			//~ }
			
		//~ }
		//~ else {return true;}
	//~ }
	//~ else
	//~ {
		//~ return true;
	//~ }		
//~ }


//~ bool Elevator::openGates(void* ptr, int var)
//~ {
	//~ IF_DYNAMIC_CAST(Elevator*, caster, ((Elevator*)ptr))
	//~ {
		//~ if( CHECK_PTR(CameraPositionPtr) && CHECK_PTR(CameraQuaternion) )
		//~ {
			//~ static std::shared_ptr<MicroWorld> cameraMicroWorld = level->getActor("Camera");
			
			//~ if( caster->elevatorOpenDoorTimer == 0 ) // первым делом запустим анимацию рук
			//~ {
				//~ printMessage("\n First step of <openGates> \n")
				
				
				//~ if( CHECK_PTR(cameraMicroWorld) )
				//~ {
					//~ cameraMicroWorld->overview = false;
					//~ ////Entity* camera = cameraMicroWorld->getEntity("Player_Hands_MESH");
					//~ if( cameraMicroWorld->components.count("Player_Hands_MESH") != 0 )	// CHECK_PTR(camera))
					//~ {
						//~ printMessage("Run hands open door" )
						//~ cameraMicroWorld->components["Player_Hands_MESH"]->setAnimation( 0, { 0.0f, {}, "Hands_Knife_Open_Door", 1, 500 }  );
						///camera->MicroWorldAnimation(0, { 0.0f, {}, "Hands_Knife_Open_Door", 1, 500 } );
					//~ }
					
				//~ }
			//~ }
			
			//~ if( caster->elevatorOpenDoorTimer > 2333 && caster->elevatorOpenDoorTimer < 2333+tick ) // когда прошло 2 секунды, запустим движение дверей и звуковое сопровождение
			//~ {
				//~ caster->launchMainMovingPartMovement(nullptr, MMP_ZERO);
				//~ caster->launchMainMovingPartMovement(nullptr, MMP_FIRST);
				
				//~ ///if( CHECK_PTR(caster->elevatorRadioSoundPtr) )
				//~ {
				//~ ///	caster->elevatorRadioSoundPtr->playSound();
				//~ }
				
				//~ caster->movePartSoundPtr->playSound();
				
				//~ ///Flow.CallSequence({"offMicroWorldMainComponent", caster->offMicroWorldMainComponent, caster, nullptr, MMP_ZERO});	//
				//~ ///Flow.CallSequence({"offMicroWorldMainComponent", caster->offMicroWorldMainComponent, caster, nullptr, MMP_FIRST});	//
			//~ }
			
			//~ caster->elevatorOpenDoorTimer += tick;
	
			//~ float mix = (float)caster->elevatorOpenDoorTimer/4500.0f;	// определим текущий коэффициет поворота
						
			//~ if(mix>1.01f)
			//~ {
				//~ caster->elevatorOpenDoorTimer = 0;		
				//~ cameraMicroWorld->overview = true;			
				//~ return true;
			//~ }
			//~ else
			//~ {
				//~ return false;
			//~ }
			
		//~ }
		//~ else {return true;}
	//~ }
	//~ else
	//~ {
		//~ return true;
	//~ }		
//~ }
	


//~ void Elevator::update()	// здесь происходит изменение параметров камеры, ОБЯЗАТЕЛЬНО делать поправку на ФПС!
//~ {
	
	//printMessage("\n")
	//~ printVec3( components["Icosphere.001"]->getLocalPosition() )
	//~ printVec3( components["Icosphere.001"]->getLocalRotation() )
	//~ printVec3( components["Icosphere.001"]->getWorldPosition() )
	//~ printVec3( components["Icosphere.001"]->getWorldRotation() )
	
	
	
	//~ /// F
	//~ if (glfwGetKey(engineWindow, GLFW_KEY_F) )
	//~ {
		//~ if(components.count("Door") != 0)
		//~ {
			//~ printMessage("Press F!")			
			//~ components["Door"]->setWorldPosition( glm::vec3(0, -1, 2) );
		//~ }
	//~ }	
	
	
	//~ /// H
	//~ if (glfwGetKey(engineWindow, GLFW_KEY_H) )
	//~ {
		//~ if(components.count("Door") != 0)
		//~ {
			//~ printMessage("Press H!")
			//~ components["Door"]->setWorldRotation( glm::quat(1,0,0,0) );
		//~ }
	//~ }	
	
//~ }



// назначим позицию для вращения штурвала гермодвери
			//~ BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;
				//~ datas.focuseDuration = 2000;
				//~ datas.currentPoint = player->getPosition();
				//~ datas.currentRot = player->getRotation();
				//~ datas.focusedPoint = components["Cabin"]->getPointWorldSpacePosition( glm::vec3(-0.5, 0, 1.7) );
				//~ datas.focusedRot = glm::quat(1, 0, 0, 0);
			
			//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function 	= std::bind( &MicroWorld::focuse, player, datas);	// главная вращающая функция
			//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> start; 
			//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> end = std::bind(&Elevator::focuse_end, this, datas) ; 
			
			//~ Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );






//~ #include "MicroWorld.cpp"
//~ #include "SoundAsset.cpp"
//~ #include "auxiliary.cpp"
//~ #include "Entity.cpp"
//~ #include "Material.cpp"
//~ #include "Shader.cpp"
//~ #include "Mesh.cpp"
//~ #include "Skeleton.cpp"
//~ #include "CollisionCalculator.cpp"
//~ #include "MovingPartsController.cpp"

//~ int main()
//~ {
	//~ return 0;
//~ }
