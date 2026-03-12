#include "WallBreach.h"

#include "CollisionDescriptor.h"
#include "macros.h"
#include "Entity.h"
#include "Level.h"
#include "Skeleton.h"
#include "SkeletonAnimation.h"
#include "Transporter.h"

WallBreach_01::WallBreach_01( glm::vec3 worldLoc, glm::quat worldRot, std::string n )
{
	MicroWorldName = n;
	
	position = worldLoc;		//назначим координаты
	rotation = worldRot;
	
	CollisionDescriptor emptyCollision;
	
	addEntity( "Wall_Breach_01", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Wall_Breach_01", emptyCollision, "MESH", { "M_Concrete_007_0.5k_x2", },  "none",  "forward" );
	addEntity( "Breach_01_slim", glm::vec3(-0.0101, 0.0, 0.3768), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Breach_01_slim", emptyCollision, "MESH", { "M_Concrete_007_0.5k_x2", },  "none",  "forward" );

	addEntity( "Dynamite", glm::vec3(0.071644, 0.01229, 1.44604), glm::quat(0.665, -0.240, -0.240, -0.665),  "SM_Dynamite_01", emptyCollision, "MESH", { "M_Dynamite_01" },  "none",  "forward"  );
	setEntityVisibility("Dynamite", false);	//	components["Dynamite"]->visible = false;


	treshold = 2.0f;
	
	status = true;

}



void WallBreach_01::logic() 	// типовая логика из класса MovingPartsController будет заменена методом из этого класса
{
	//~ static glm::vec3 dynamiteCoord = glm::vec3(0.073115, 0.026416, 1.49914);
	//~ runtimePositionChange(dynamiteCoord);
				
								
	if(CHECK_PTR(CameraPositionPtr) && bWallBreachInitialized )
	{
		glm::vec3 actor_to_breach = glm::normalize( getPosition()- *CameraPositionPtr);
		glm::vec3 actor_forward = (*CameraAxisPtr)[0];
		
		float dot = glm::dot(actor_to_breach, actor_forward  );
		//~ printVariable(dot)
		
		if(dot > 0.85 && !bWallBreachIdeaGiven)
		{
			bWallBreachIdeaGiven = true;
			
			auto player = level->getActor("Camera");
			if(player)
			{	
				
				
				glm::vec3 target = glm::vec3(0,0,0);	//FUNC( getEntityPosition( "Dynamite" ) )
				if( components.contains("Dynamite") )
				{
					target = components["Dynamite"]->getWorldPosition();
				}
				player->focuseToSpecifiedPoint( target , 2000, 														//	 2000,
											std::bind(&WallBreach_01::wallBreachFocuse_start, this),
											std::bind(&WallBreach_01::wallBreachFocuse_end, this)	);	
			}
		}
	}
	
	auto chk = [&]()	{ 	if( CHECK_PTR(CameraPositionPtr) ) 		{	float length = glm::length(*CameraPositionPtr - position);		return  length < treshold ? true : false;		} 
							else 			{	printMessage("Camera ptr is invalid")			return true;	} 		};
	
		
	if ( bWallBreachIdeaGiven && !bWallBreachActorIsNear )
	{
		auto player = level->getActor("Camera");
		
		if(player && chk() )
		{
			bWallBreachActorIsNear = true;
			
			player->goAndFocuseToSpecifiedPoint( components["Wall_Breach_01"]->getPointWorldSpacePosition( glm::vec3(-200.588549, 0, 1.59932) ), 
												 components["Wall_Breach_01"]->getPointWorldSpacePosition( glm::vec3(0.588549, 0, 1.59932) ),
												2000, std::bind(&WallBreach_01::wallBreachApproaching_start, this), std::bind(&WallBreach_01::wallBreachApproaching_end, this) );	
		}
	}
	
}


void WallBreach_01::wallBreachFocuse_start()
{
	//~ printVarComment("wallBreachFocuse_start clb for: ", MicroWorldName)
	sound_IdeaForBlowing->playSound( [](){}, [](){} );
	
	auto player = level->getActor("Camera");
	if(player)
	{
		player->overview = false;
	}
}

void WallBreach_01::wallBreachFocuse_end()
{
	//~ printVarComment("wallBreachFocuse_end clb for: ", MicroWorldName)
	
	auto player = level->getActor("Camera");
	if(player)
	{
		player->overview = true;
	}
}



void WallBreach_01::wallBreachApproaching_start()
{
	//~ printVarComment("wallBreachApproaching_start clb for: ", MicroWorldName)
	
	auto player = level->getActor("Camera");
	if(player)
	{
		player->overview = false;
	}
}

void WallBreach_01::wallBreachApproaching_end()
{
	//~ printVarComment("wallBreachApproaching_end clb for: ", MicroWorldName)
	
	BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;	
	//~ datas.var = MMP_FIRST;
	
	std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function = std::bind( &WallBreach_01::dynamiteInstallSequence, this, datas);	// главная вращающая функция
	auto start = std::bind( &WallBreach_01::dynamiteInstall_Start, this);		// колбек начала
	auto end = std::bind( &WallBreach_01::dynamiteInstall_End, this);		// колбек конца
		
	Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
	
	
	
}

bool WallBreach_01::dynamiteInstallSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
{
	int duration = 5000;
	auto camera = level->getActor("Camera");
	if(camera)
	{
		if(camera->components.count(camera->mainVisualEntity) != 0)
		{
			duration = camera->components[camera->mainVisualEntity]->entitySkeletonPtr->sklAnimations["Hands_Install_Dynamite"]->getDuration();	//.count( newAnim.animName )
		}
	}
	
	if(iDynamiteInstallCounter > duration)
	{
		iDynamiteInstallCounter = 0;
		return true;
	}
	
	if(iDynamiteInstallCounter > 3300)
	{
		FUNC( setEntityVisibility("Dynamite", true ) )	//components["Dynamite"]->visible = true;
	}
	
	iDynamiteInstallCounter+= tick;
	
	return false;
	
}

void WallBreach_01::dynamiteInstall_Start()
{
	//~ printMessage("dynamiteInstallStart")
	
	std::shared_ptr<MicroWorld> cameraMicroWorld = level->getActor("Camera");
	if(cameraMicroWorld)
	{
		animStateConfig handsAnimation, noSecondAnimation;
			handsAnimation.mixCoeff = 0.0f;
			handsAnimation.mask = {};
			handsAnimation.animName = "Hands_Install_Dynamite";
			handsAnimation.repeatCount = 1;
			handsAnimation.blendingTimeIn = 500;
		FUNC( cameraMicroWorld->setComponentAnimation( cameraMicroWorld->mainVisualEntity, 0, handsAnimation, noSecondAnimation ) )	
		
	}
}

void WallBreach_01::dynamiteInstall_End()
{
	//~ printMessage("dynamiteInstallEnd")
	
	sound_Bomb_planted->playSound( [](){}, [](){} );
	
	//~ sound_Bomb_timer_launched->playSound( std::bind(&WallBreach_01::bombLaunch_Start, this), std::bind(&WallBreach_01::bombLaunch_End, this) );	
	// запуск нового события	
	BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;	
	//datas.var = MMP_FIRST;
	
	std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function = std::bind( &WallBreach_01::bombLaunchSequence, this, datas);	// главная вращающая функция
	auto start = std::bind( &WallBreach_01::bombLaunch_Start, this);		// колбек начала
	auto end = std::bind( &WallBreach_01::bombLaunch_End, this);		// колбек конца
	Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
	
	
	auto player = level->getActor("Camera");
	if(player)
	{
		player->overview = true;
	}
}



bool WallBreach_01::bombLaunchSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
{
	if(iDynamiteTimerCounter > 5000)
	{
		iDynamiteTimerCounter = 0;
		return true;
	}
	iDynamiteTimerCounter+= tick;
	
	return false;
}

void WallBreach_01::bombLaunch_Start()
{
	//~ printMessage("bombLaunch_Start")
	
	sound_Bomb_timer_launched->playSound( [](){}, [](){} );	

}

void WallBreach_01::bombLaunch_End()
{
	//~ printMessage("bombLaunch_End")
	
	setEntityVisibility("Breach_01_slim", false);	//components["Breach_01_slim"]->visible = false;
	
	level->spawnActor<Transporter>("Transporter_01", getPosition()+glm::vec3(0,0,0.1), glm::quat(0.707, 0, 0, -0.707) );	// через пролом
	
	setEntityVisibility("Dynamite", false);	//components["Dynamite"]->visible = false;
}


//////

bool WallBreach_01::focuse_end(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
{
	//~ printVarComment("End focuse script for: ", MicroWorldName)
	
	//~ datas.var = MMP_ZERO;
	//~ Flow.CallSequence( { std::move(std::bind( &MovingPartsController::rotateUniversal, this, datas)), std::move(datas) } );
	
	return true;
}

















			//~ // назначим позицию для вращения штурвала гермодвери
			//~ BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;
				//~ datas.focuseDuration = 2000;
				//~ datas.currentPoint = player->getPosition();
				//~ datas.currentRot = player->getRotation();
				//~ datas.focusedPoint = components["Breach_01_slim"]->getPointWorldSpacePosition( glm::vec3(0.572, 0, 1.651) );
				//~ datas.focusedRot = glm::quat(0.707,0,0,-0.707);
			
			//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function 	= std::bind( &MicroWorld::focuse, player, datas);	// главная вращающая функция
			//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> start; 
			//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> end = std::bind(&WallBreach_01::focuse_end, this, datas) ; 
			
			//~ Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );


////////////////////
//~ // назначим позицию для вращения штурвала гермодвери
				//~ BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;
					//~ datas.focuseDuration = 2000;
					//~ datas.currentPoint = player->getPosition();
					//~ datas.currentRot = player->getRotation();
					//~ datas.focusedPoint = datas.currentPoint;			//components["Wall_Breach_01"]->getPointWorldSpacePosition( glm::vec3(1, 0, 1.87) );
					//~ datas.focusedRot = player->getQuaternionFocusedToPoint( components["Dynamite"]->getWorldPosition() ); //glm::quat(1,0,0,0);
				
				//~ // запустим приближение				
				//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function 	= std::bind( &MicroWorld::focuse, 	player, datas);	// фокусирующая функция
				//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> start;// 		= std::bind( &Gate_01::approachingToopeningGate_start, this, datas); 
				//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> end;// 		= std::bind( &WallBreach_01::focuse_end, this, datas) ; 
				//~ Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );

 
