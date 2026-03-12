#include "EXO_Soldier.h"
#include "AnimationController.h"

#include "Weapon.h"
#include "CollisionDescriptor.h"
#include "Entity.h"
#include "Level.h"
#include "macros.h"
#include "auxiliary.h"
#include "Laboratory.h"

EXO_Soldier::EXO_Soldier( glm::vec3 worldLoc, glm::quat worldRot, std::string n )
{
	MicroWorldName = n;
	
	position = worldLoc;		//назначим координаты
	rotation = worldRot;
	
	
	CollisionDescriptor npcCollision;
	npcCollision.collisionRadius = 3.8f;
	npcCollision.collisionType = COMPLEX_COLLISION;
	npcCollision.complexCollisionFileName = "CB_Orc_01";
	
	// PLAYER_COLLISION в функции update ежекадрово принимает позицию на 1 метр ниже координат комбо!
	addEntity( "AABB_Box", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "Cube", npcCollision, "MESH", { "M_Default_0_5k", },  "none",  "forward" );
	setEntityVisibility( "AABB_Box", false );	//components["AABB_Box"]->visible = false;
	
	
		
	addEntity( "ExoSoldier_01", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SKM_ExoSoldier_01", {  }, "MESH", { "M_Exo_Soldier_01_0_5k",  "M_Default_0_5k", },  "none",  "forward" );
	
	mainCollisionEntity = "AABB_Box";
	mainVisualEntity = "ExoSoldier_01";


	idleAnimationName 		= "man_idle";
	walkAnimationName 		= "man_walk"	;
	attackAnimationName 	= "man_punch"	;
	dieAnimationName 		= "man_death";

	idleWalkingSpeed = 1.5f;

	animStateConfig orcIdleAnimation;
		orcIdleAnimation.mixCoeff = 0.0f;
		orcIdleAnimation.mask = {};
		orcIdleAnimation.animName = idleAnimationName;
		orcIdleAnimation.repeatCount = -1;
		orcIdleAnimation.blendingTimeIn = 500;
	
	components[mainVisualEntity]->setAnimation(0, orcIdleAnimation, orcIdleAnimation	);	

	// монстр
	
	status = components.size()==2;	//true;
}


void EXO_Soldier::logic()
{
	if( !armed )	// && level->getTaggedActorNearPoint("Weapon", position, 3.0f) )
	{
		
		// new
		std::shared_ptr<Weapon> wpn = std::dynamic_pointer_cast<Weapon>( level->getTaggedActorNearPoint("Weapon", position, 3.0f) );
		
		// ///Weapon* wpn = dynamic_cast<Weapon*>( level->getTaggedActorNearPoint("Weapon", caster->position, radius) );
		
		if( CHECK_PTR(wpn) && wpn->parentMicroWorld == nullptr && wpn->canBeTaken )		// XXX внимательнее с условием (wpn->parentMicroWorld == nullptr)
		{
			//~ printMessage("Take weapon")
			wpn->attachTo( this, mainVisualEntity, "mixamorig:RightHand", wpn->getSocketPosition(), wpn->getSocketRotationXYZW() ); //vec4(0.5317, -0.087, -0.089, 1), vec4(-0.272, 0.066, 0.156, 0.947));
			
			armed = true;
			
			wpn->components[ wpn->mainCollisionEntity ] -> disableCollision();	// при подборе оружию отключаем коллизии
				
			////sound_takeWeapon->playSound();
		}
		
		
		// old
		// Weapon* wpn = dynamic_cast<Weapon*>( level->getTaggedActorNearPoint("Weapon", caster->worldLocation));
			
		// if(wpn!=nullptr && caster->childMicroWorld == nullptr && wpn->parentMicroWorld == nullptr)
		// {
			// wpn->MicroWorldAttachTo(caster, "ExoSoldier_01", "mixamorig:RightHand", glm::vec4(0.009, -0.85, 1.345, 1), glm::vec4(-0.56, 0.476, -0.5, 0.458)); 
		// }
	}
	
	
	components[mainCollisionEntity]->setWorldRotation( glm::quat(1,0,0,0) );
	
	// сугубо скриптовый персонаж
	
	if( !triggered && glm::length(level->getActor("Camera")->getPosition() - position) < 2 )		//glfwGetKey(engineWindow, GLFW_KEY_E) &&
	{
		//~ printMessage("Soldier script")
		triggered = true;
		
		sound_1->playSound(std::bind(&EXO_Soldier::meeting_start, this), std::bind(&EXO_Soldier::meeting_end, this) );
	}
	
	//~ if(glfwGetKey(engineWindow, GLFW_KEY_T))
	//~ {
		//~ triggered = false;
		//~ position = glm::vec3(13, -1, 0.1);
		//~ rotation = glm::quat(0.707, 0, 0, 0.707);
		//~ rotating_start();
	//~ }
	
	static glm::vec3 foor = glm::vec3(idleWalkingSpeed,0,0);
	runtimePositionChange(foor);
	
	idleWalkingSpeed = foor.x;
}

void EXO_Soldier::meeting_start()
{
	//~ printMessage("Start speech")
}
void EXO_Soldier::meeting_end()
{
	//~ printMessage("end speech")
	
	targetPoint = glm::vec3(19, 0, -2.4);	//getRandomPointInRadius( glm::vec3(7, 8, 0), 7.0f);	//
	focuseToSpecifiedPoint( targetPoint, 1000, std::bind(&EXO_Soldier::rotating_start, this), std::bind(&EXO_Soldier::rotating_end, this)	);
}


void EXO_Soldier::rotating_start()
{
	//~ printMessage("start rotation")
	
	//if(alive)
	{
		animStateConfig orcAnimation;
			orcAnimation.mixCoeff = 0.0f;
			orcAnimation.mask = {};
			orcAnimation.animName = idleAnimationName;
			orcAnimation.repeatCount = -1;
			orcAnimation.blendingTimeIn = 500;
		
		components[mainVisualEntity]->setAnimation(0, orcAnimation, orcAnimation	);	
	}
}
void EXO_Soldier::rotating_end()
{
	//~ printMessage("end rotation")

	//~ animStateConfig orcIdleAnimation;
		//~ orcIdleAnimation.mixCoeff = 0.0f;
		//~ orcIdleAnimation.mask = {};
		//~ orcIdleAnimation.animName = walkAnimationName;
		//~ orcIdleAnimation.repeatCount = -1;
		//~ orcIdleAnimation.blendingTimeIn = 500;
		//~ orcIdleAnimation.end_clb = std::bind(&EXO_Soldier::foo, this);
		
	//~ components[mainVisualEntity]->setAnimation(0, orcIdleAnimation, orcIdleAnimation	);	
	

	BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;		
	std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function = std::bind( &EXO_Soldier::patrolingSequence, this, datas);	// главная функция
	auto start = std::bind( &EXO_Soldier::evac_done_start, this);		// колбек начала
	auto end = std::bind( &EXO_Soldier::evac_done_end, this);		// колбек конца
		
	Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
}


void EXO_Soldier::foo()
{
	//~ printMessage("foo")
	//triggered = false;
	
	sound_NPCwalking->playSound( [](){}, [](){} );
	
}


bool EXO_Soldier::patrolingSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
{
	//~ printMessage("\nPATROLING")
	//~ printVec3(position)
	//~ printVec3(targetPoint)
	
	restartIntervalTimer += tick;
	
	if(restartIntervalTimer > 3000)
	{
		restartIntervalTimer = 0;
		//~ printMessage("Watchdog patrol")
		//~ position = glm::vec3(14, -2, 0.1);
		//~ rotation = glm::quat(0.707, 0, 0, 0.707);
		//~ triggered = false;
		
		//~ DEBUG_INFO
		//~ return true;
	}
	
	if( alive && glm::length( targetPoint - position ) > 0.5f )	// покуда расстояние до цели больше пороговых 0.5 м
	{
		//~ printMessage("\t walking")
		//~ printVariable(idleWalkingSpeed)
		
		glm::vec3 direction = targetPoint - position;
		
		//~ DEBUG_INFO
		moveInDirection( mainCollisionEntity, direction, idleWalkingSpeed * float(tick)/1000.0f );
		moveInDirection( mainCollisionEntity, -glm::vec3(0,0,1), idleWalkingSpeed * float(tick)/1000.0f );
		
		////position += idleWalkingSpeed * float(tick)/1000.0f * glm::normalize( targetPoint - position );	// добавим смещения
		
		
		static int step = 0;
		auto lab = dynamic_pointer_cast<Laboratory> (level);			//	Laboratory* lab = dynamic_cast<Laboratory*>(level);
		
		if(step > 300 && lab != nullptr && lab->sound4done)
		{
			step=0;
			
		}
		step+=tick;
		
		
		
		return false;
	}
	else
	{
		return true;
	}
	// check if see player
	
	
	//~ return false;
}


void EXO_Soldier::evac_done_start()
{
	//~ printMessage("start going")
	
	animStateConfig orcIdleAnimation;
		orcIdleAnimation.mixCoeff = 0.0f;
		orcIdleAnimation.mask = {};
		orcIdleAnimation.animName = walkAnimationName;
		orcIdleAnimation.repeatCount = -1;
		orcIdleAnimation.blendingTimeIn = 150;
		
		orcIdleAnimation.middle_clb = std::bind(&EXO_Soldier::foo, this);
		orcIdleAnimation.end_clb = std::bind(&EXO_Soldier::foo, this);
			
	components[mainVisualEntity]->setAnimation(0, orcIdleAnimation, orcIdleAnimation	);	
}

void EXO_Soldier::evac_done_end()
{
	//~ printMessage("mission complete")
	///triggered = false;
	
	animStateConfig orcAnimation;
		orcAnimation.mixCoeff = 0.0f;
		orcAnimation.mask = {};
		orcAnimation.animName = idleAnimationName;
		orcAnimation.repeatCount = -1;
		orcAnimation.blendingTimeIn = 500;
	
	components[mainVisualEntity]->setAnimation(0, orcAnimation, orcAnimation	);	
	
	sound_2->playSound( [](){}, [](){} );
}


	
	
//~ if( glfwGetKey(engineWindow, GLFW_KEY_1) && !keys[0] )
//~ {
	//~ keys[0] = true;
	//~ animStateConfig orcIdleAnimation;
		//~ orcIdleAnimation.mixCoeff = 0.0f;
		//~ orcIdleAnimation.mask = {};
		//~ orcIdleAnimation.animName = idleAnimationName;
		//~ orcIdleAnimation.repeatCount = -1;
		//~ orcIdleAnimation.blendingTimeIn = 500;
	
	//~ components[mainVisualEntity]->setAnimation(0, orcIdleAnimation, orcIdleAnimation	);	
//~ }
//~ if( !glfwGetKey(engineWindow, GLFW_KEY_1) ) {keys[0] = false;}



//~ if( glfwGetKey(engineWindow, GLFW_KEY_2) && !keys[1] )
//~ {
	//~ keys[1] = true;
	//~ animStateConfig orcIdleAnimation;
		//~ orcIdleAnimation.mixCoeff = 0.0f;
		//~ orcIdleAnimation.mask = {};
		//~ orcIdleAnimation.animName = walkAnimationName;
		//~ orcIdleAnimation.repeatCount = -1;
		//~ orcIdleAnimation.blendingTimeIn = 500;
	
	//~ components[mainVisualEntity]->setAnimation(0, orcIdleAnimation, orcIdleAnimation	);	
//~ }
//~ if( !glfwGetKey(engineWindow, GLFW_KEY_2) ) {keys[1] = false;}



//~ if( glfwGetKey(engineWindow, GLFW_KEY_3) && !keys[2] )
//~ {
	//~ keys[2] = true;
	//~ animStateConfig orcIdleAnimation;
		//~ orcIdleAnimation.mixCoeff = 0.0f;
		//~ orcIdleAnimation.mask = {};
		//~ orcIdleAnimation.animName = attackAnimationName;
		//~ orcIdleAnimation.repeatCount = -1;
		//~ orcIdleAnimation.blendingTimeIn = 500;
	
	//~ components[mainVisualEntity]->setAnimation(0, orcIdleAnimation, orcIdleAnimation	);	
//~ }
//~ if( !glfwGetKey(engineWindow, GLFW_KEY_3) ) {keys[2] = false;}




//~ if( glfwGetKey(engineWindow, GLFW_KEY_4) && !keys[3] )
//~ {
	//~ keys[3] = true;
	//~ animStateConfig orcIdleAnimation;
		//~ orcIdleAnimation.mixCoeff = 0.0f;
		//~ orcIdleAnimation.mask = {};
		//~ orcIdleAnimation.animName = dieAnimationName;
		//~ orcIdleAnimation.repeatCount = -1;
		//~ orcIdleAnimation.blendingTimeIn = 500;
	
	//~ components[mainVisualEntity]->setAnimation(0, orcIdleAnimation, orcIdleAnimation	);	
//~ }
//~ if( !glfwGetKey(engineWindow, GLFW_KEY_4) ) {keys[3] = false;}
