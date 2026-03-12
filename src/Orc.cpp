#include "Orc.h"
#include "AnimationController.h"
#include "Level.h"
#include "CollisionDescriptor.h"
#include "Entity.h"
#include "Door_01.h"
#include "macros.h"
#include "global.h"
#include "Camera.h"


Orc::Orc( glm::vec3 worldLoc, glm::quat worldRot, std::string n )
{
	MicroWorldName = n;
	
	position = worldLoc;		//назначим координаты
	rotation = worldRot;
	
	
	CollisionDescriptor npcCollision;
	npcCollision.collisionRadius = 3.8f;
	npcCollision.collisionType = COMPLEX_COLLISION;
	npcCollision.complexCollisionFileName = "CB_Orc_01";
	
	// PLAYER_COLLISION в функции update ежекадрово принимает позицию на 1 метр ниже координат комбо!
	bool collisionShell = addEntity( "NPC_COLLISION", glm::vec3(0,0,0), glm::quat(1.0, 0.0, 0.0, 0.0),  "CB_Orc_01", npcCollision, "MESH", {"M_Ork_01_0_5k"},  "none",  "forward" );	//таким образом можно одновременно и визуализировать оболочку
		setEntityVisibility( "NPC_COLLISION", false );	//components["NPC_COLLISION"]->visible = false;
	
	
	mainCollisionEntity = "NPC_COLLISION";
	
	bool mesh = addEntity( "Ork_01", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SKM_Ork_01", {  }, "MESH", { "M_Ork_01_0_5k", },  "none",  "forward" );
	mainVisualEntity = "Ork_01";


	idleAnimationName 		= "Ork_idle";
	walkAnimationName 		= "Ork_walk"	;
	attackAnimationName 	= "Ork_attack"	;
	dieAnimationName 		= "Ork_die";


	animStateConfig orcIdleAnimation;
		orcIdleAnimation.mixCoeff = 0.0f;
		orcIdleAnimation.mask = {};
		orcIdleAnimation.animName = "Ork_idle";
		orcIdleAnimation.repeatCount = -1;
		orcIdleAnimation.blendingTimeIn = 500;
	
	components["Ork_01"]->setAnimation(0, orcIdleAnimation, orcIdleAnimation	);	

	// монстр
	status = (collisionShell && mesh);
	//~ if( !collisionShell ) {status = false;}
	//~ if( !mesh ) {status = false;}


	//~ position = glm::vec3(32, 14, 1);				// перемещаем в основное место
	//~ meeting = true;	// разрешаем основную логику	

}

void Orc::logic() 	// типовая логика из класса MovingPartsController будет заменена методом из этого класса
{		
	if( spotted ) 
	{
		spotted = false;
		targetPoint = glm::vec3(-6, 21, 2.7);		//player->getPosition();	//getRandomPointInRadius( glm::vec3(7, 8, 0), 7.0f);
		focuseToSpecifiedPoint( targetPoint, 1000, std::bind(&Orc::orcRotationIdle_start, this), std::bind(&Orc::orcRotationIdle_end, this)	);
	}
	
			
	if( meeting && bIsTargetInRadius("Camera", 12.0f) && !flag1 )
	{
		flag1 = true;
		
		//~ printMessage("Start effect arriving, play sound")
		
		screamer2SoundPtr->playSound( [](){}, [](){} );			// однократно воспроизведем звук
		
		auto player = level->getActor("Camera");
		if(player)
		{
			player->overview = false;										// заблокируем камеру
						
			player->focuseToSpecifiedPoint( getPosition(), 2500,			// запуск цикла фокусировки этого комбо на указанную точку и явное указание безаргументных колбеков
											std::bind(&Orc::playerFocuseAtOrk_start, this),
											std::bind(&Orc::playerFocuseAtOrk_end, this)	);	
			
			rotation = getHorizontalQuaternionFocusedToPoint( player->getPosition() );	// повернем орка в направлении игрока
		}
	}
	
	//------------------------------------------------------------------
	
	if( bOrcArrived && logicChangeable )
	{
		auto player = level->getActor("Camera");
		if(player)
		{
			//~ printMessage("Change logic!")
			logicChangeable = false;
			
			targetPoint = player->getPosition() - glm::vec3(0,0,1.5);	//getRandomPointInRadius( glm::vec3(7, 8, 0), 7.0f);
			
			focuseToSpecifiedPoint( targetPoint, 70, std::bind(&Orc::orcRotation_start, this), std::bind(&Orc::orcRotation_end, this)	);
		}	
	}
}



void Orc::playerFocuseAtOrk_start()
{
	//~ printMessage("Player focusing on target...")
}

void Orc::playerFocuseAtOrk_end()				// сфокусировались на орке
{
	//~ printMessage("\tPlayer focused!")

	BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;		
	std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function = std::bind( &Orc::monsterArrivingSequence, this, datas);	// главная функция
	auto start = std::bind( &Orc::monsterArriving_start, this);		// колбек начала
	auto end = std::bind( &Orc::monsterArriving_end, this);		// колбек конца
	Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
}



void Orc::monsterArriving_start()
{
	//~ printMessage("Monster spectacular falling...")
}
bool Orc::monsterArrivingSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)	// спускаем орка со второго этажа
{
	if( glm::length( glm::vec3(32, 14, -2.3) - position ) > 0.2f )	// покуда расстояние до цели больше пороговых 0.2 м
	{
		position += glm::vec3(0,0,-1) * 2.5f*idleWalkingSpeed * float(tick)/1000.0f ;
		//moveInDirection( mainCollisionEntity, -glm::vec3(0,0,1), 2.5f*idleWalkingSpeed * float(tick)/1000.0f );
		return false;
	}
	else
	{
		return true;
	}
}
void Orc::monsterArriving_end()
{
	//~ printMessage("\tMonster has landing")
	jumpSoundPtr->playSound( [](){}, [](){} );
	
	auto player = level->getActor("Camera");
	if(player)
	{
		player->overview = true;				// возвращаем игроку управление, орк появился полностью
		// battle music play
	}
	bOrcArrived = true;	
	
	
	// закрываем и блокируем двери
	std::shared_ptr<Door_01> door_2 = dynamic_pointer_cast<Door_01>( level->getActor("Door_02") );
	if(door_2)
	{
		door_2->locked = true;
		if(door_2->opened)
		{
			door_2->launchMainMovingPartMovement(0, std::bind(&Door_01::opening_start, door_2), std::bind(&Door_01::opening_end, door_2));
		}
	}
	
	std::shared_ptr<Door_01> door_3 = dynamic_pointer_cast<Door_01>( level->getActor("Door_03") );
	if(door_3)
	{
		door_3->locked = true;
		if(door_3->opened)
		{
			door_3->launchMainMovingPartMovement(0, std::bind(&Door_01::opening_start, door_3), std::bind(&Door_01::opening_end, door_3));
		}
	}
}







void Orc::orcRotation_start()
{
	//~ printMessage("Rotating to player...")
}
void Orc::orcRotation_end()
{
	//~ printMessage("\tRotated!")
	
	screamSoundPtr->playSound( [](){}, [](){} );
	
	if( glm::length(targetPoint-position)	> 3.0f )	//!bIsTargetInRadius("Camera", 3.0f) )	// если игрок далеко
	{
		printMessage("\tPlayer too far")
		BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;		
		std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function = std::bind( &Orc::orcPursuitingSequence, this, datas);	// главная функция
		auto start = std::bind( &Orc::orcPursuiting_start, this);		// колбек начала
		auto end = std::bind( &Orc::orcPursuiting_end, this);		// колбек конца
			
		Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
	}
	else // если игрок рядом, надо сразу перейти к анимации атаки
	{
		printMessage("\tPlayer near")
	}
	
	
}


void Orc::orcRotationIdle_start()
{
	
}
void Orc::orcRotationIdle_end()
{
	screamSoundPtr->playSound( [](){}, [](){} );
	
	//~ printVec3(targetPoint)
	//~ printVec3(position)
	
	if( glm::length(targetPoint-position)	> 3.0f )	//!bIsTargetInRadius("Camera", 3.0f) )	// если игрок далеко
	{
		rotation = getHorizontalQuaternionFocusedToPoint( targetPoint );	// playerPos );
		
		//~ printMessage("\t target too far")
		BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;		
		std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function = std::bind( &Orc::orcPatrolingSequence, this, datas);	// главная функция
		auto start = std::bind( &Orc::orcPursuiting_start, this);		// колбек начала
		auto end = std::bind( &Orc::orcPatrolingSequence_end, this);		// колбек конца
			
		Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
	}
	else // если игрок рядом, надо сразу перейти к анимации атаки
	{
		//~ printMessage("\t Target near")
	}
}
bool Orc::orcPatrolingSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
{
	glm::vec3 direction = targetPoint - position;
		
	if( alive && glm::length( direction ) > 2.0f )	// покуда расстояние до цели больше пороговых 0.5 м
	{
		//~ position += 0.85f * direction * idleWalkingSpeed * float(tick)/1000.0f;
		
		moveInDirection( mainCollisionEntity, direction, 0.85f * idleWalkingSpeed * float(tick)/1000.0f );
		//~ moveInDirection( mainCollisionEntity, -glm::vec3(0,0,1), idleWalkingSpeed * float(tick)/1000.0f );
		return false;
	}
	else 		// достигнув цели, обновим параметры состояния
	{
		return true;
	}
}

void Orc::orcPatrolingSequence_end()
{
	animStateConfig orcAnimation;
			orcAnimation.mixCoeff = 0.0f;
			orcAnimation.mask = {};
			orcAnimation.animName = idleAnimationName;
			orcAnimation.repeatCount = -1;
			orcAnimation.blendingTimeIn = 100;
			
			//orcAnimation.end_clb = std::bind( &Orc::hitAnimation_end, this);	// разрешим обновлять логику спустя цикл анимации
		
		components[mainVisualEntity]->setAnimation(0, orcAnimation, orcAnimation	);
		
	position = glm::vec3(32, 14, 1);				// перемещаем в основное место
	meeting = true;	// разрешаем основную логику	
}


void Orc::orcPursuiting_start()
{
	printMessage("Pursuiting player...")
	
	if(alive)
	{
		animStateConfig orcAnimation;
			orcAnimation.mixCoeff = 0.0f;
			orcAnimation.mask = {};
			orcAnimation.animName = walkAnimationName;
			orcAnimation.repeatCount = -1;
			orcAnimation.blendingTimeIn = 500;
			
			orcAnimation.middle_clb = std::bind(&NPC::stepSound, this);
			orcAnimation.end_clb = std::bind(&NPC::stepSound, this);
		
		components[mainVisualEntity]->setAnimation(0, orcAnimation, orcAnimation	);	
	}
}


bool Orc::orcPursuitingSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
{
	//~ iOrcPursuitingWatchDog+= tick;
	//~ if( iOrcPursuitingWatchDog > 500 )
	//~ {
		//~ iOrcPursuitingWatchDog = 0;
	//~ }
	//~ printMessage("pursuit")
	
	glm::vec3 npcToPlayerVec = glm::normalize( *CameraPositionPtr - getPosition() );	// вектор на игрока
	glm::vec3 npcForwardVec = m3VectorsForwardLeftUp[0];	// впередсмотрящий вектор монстра
	//~ float dot = glm::dot(npcForwardVec, npcToPlayerVec  );
	
	//if(dot > std::cos(FOV/2*3.14/180) ) // пока игрок в секторе обзора
	{
		//~ std::shared_ptr<MicroWorld> caster = traceRay( npcToPlayerVec );
		auto caster = dynamic_pointer_cast<Camera>(level->getActor("Camera"));
		if(caster)			
		{
			//~ if(caster->MicroWorldName == "Camera")	// и если виден именно игрок!
			{
				//glm::vec3 playerPos = glm::vec3( caster->getPosition().x, caster->getPosition().y, 0.1);
				
				glm::vec3 direction = targetPoint - position;
				
				if( alive && glm::length( direction ) > 2.5f )	// покуда расстояние до цели больше пороговых 0.5 м
				{
					rotation = getHorizontalQuaternionFocusedToPoint( targetPoint );	// playerPos );
					targetPoint = glm::vec3( caster->getPosition().x, caster->getPosition().y, 0.1);	// обновим целевую точку
					
					moveInDirection( mainCollisionEntity, direction, 1.5*idleWalkingSpeed * float(tick)/1000.0f );
					moveInDirection( mainCollisionEntity, -glm::vec3(0,0,1), idleWalkingSpeed * float(tick)/1000.0f );
					attacking = false;
					return false;
				}
				else 		// достигнув цели, обновим параметры состояния
				{
					attacking = true;	
					return true;
				}
				
			}
			//~ else 		// если виден не игрок
			//~ {
				//~ seePlayer = false;
				//~ return true;		// заканчиваем цикл преследования, т.к. видим НЕ игрока
			//~ }
		}
		else
		{
			return false;
		}
	}
	//~ else 		// если игрок вне сектора обзора
	//~ {
		//~ seePlayer = false;
		//~ return true;		// тоже заканчиваем цикл преследования, т.к. не видим игрока
	//~ }
}

void Orc::orcPursuiting_end()
{
	printMessage("Player achieved, attack!")

	//logicChangeable = true;
	
	if(alive)
	{
		if(attacking)
		{
			animStateConfig orcAnimation;
				orcAnimation.mixCoeff = 0.0f;
				orcAnimation.mask = {};
				orcAnimation.animName = attacking ? attackAnimationName : idleAnimationName;
				orcAnimation.repeatCount = -1;
				orcAnimation.blendingTimeIn = 100;
				
				orcAnimation.end_clb = std::bind( &Orc::hitAnimation_end, this);	// разрешим обновлять логику спустя цикл анимации
			
			components[mainVisualEntity]->setAnimation(0, orcAnimation, orcAnimation	);	
		}
		else
		{
			logicChangeable = true;	// или сразу
		}
	}

}


void Orc::hitAnimation_end()
{
	logicChangeable = true;
	
	hitSound->playSound( [](){}, [](){} );
	
	std::shared_ptr<MicroWorld> caster = traceRay( m3VectorsForwardLeftUp[0] );		// определим, в кого попали
	
	if( caster && caster->hasTag("Camera") )
	{
		std::shared_ptr<Camera> player = dynamic_pointer_cast<Camera>(caster);
		if(player)
		{
			player->sound_Pain->playSound( [](){}, [](){} );
		}
	}
	
	
}


////////

void Orc::monsterArrivingSound_start(){}
void Orc::monsterArrivingSound_end(){}

