#include "NPC.h"
#include <cmath>

#include "AnimationController.h"

#include "macros.h"
#include "Entity.h"
#include "Laboratory.h"
#include "Door_01.h"
#include "Skeleton.h"
#include "SkeletonAnimation.h"


NPC::NPC( glm::vec3 worldLoc, glm::quat worldRot, std::string n )
{
	MicroWorldName = n;
	
	position = worldLoc;		//назначим координаты
	rotation = worldRot;

	// монстр
}


void NPC::logic() 	// типовая логика из класса MovingPartsController будет заменена методом из этого класса
{				
	//~ printMessage("NPC logic")

	
	
	glm::vec3 npcToPlayerVec = glm::normalize( *CameraPositionPtr - getPosition() );	// вектор на игрока
	glm::vec3 npcForwardVec = m3VectorsForwardLeftUp[0];	// впередсмотрящий вектор монстра
	float dot = glm::dot(npcForwardVec, npcToPlayerVec  );
	
	if(dot > std::cos(FOV/2*3.14/180)  && logicChangeable)
	{
		std::shared_ptr<MicroWorld> caster = traceRay( npcToPlayerVec );
		if(caster && caster->MicroWorldName == "Camera")
		{
			//printVarComment("Casted combo:", caster->MicroWorldName )
			sound_SeePlayer->playSound( [](){}, [](){} );
			
			logicChangeable = false;
			
			targetPoint = glm::vec3( caster->getPosition().x, caster->getPosition().y, 0.1);	// getRandomPointInRadius( glm::vec3(7, 8, 0), 5.0f);
			focuseToSpecifiedPoint( targetPoint, 700, std::bind(&NPC::rotating_start, this), std::bind(&NPC::rotating_end, this)	);
		}
	}
	
	if(dot < std::cos(FOV/2*3.14/180) && !logicChangeable)	// как только вышел из поля зрения - неважно уже, спрятан игрок за чем-либо
	{
		logicChangeable = true;
		
		animStateConfig orcAnimation;
				orcAnimation.mixCoeff = 0.0f;
				orcAnimation.mask = {};
				orcAnimation.animName = idleAnimationName;
				orcAnimation.repeatCount = -1;
				orcAnimation.blendingTimeIn = 500;			
		components[mainVisualEntity]->setAnimation(0, orcAnimation, orcAnimation	);	
	}
		
}




void NPC::empty_clb_start()	{	}
void NPC::empty_clb_end()	{	}

void NPC::rotating_start()
{
	printMessage("start rotating")
}
void NPC::rotating_end()
{
	printMessage("end rotating")
	
	if(alive)	// если жив и закончил поворачиваться в сторону цели
	{
		
		
		BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;		
		std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function = std::bind( &NPC::patrolingSequence, this, datas);	// главная функция
		auto start = std::bind( &NPC::patrolling_start, this);		// колбек начала
		auto end = std::bind( &NPC::patrolling_end, this);		// колбек конца
			
		Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
		
		///seePlayer = false; // XXX этот флаг надо переименовать
	}
	
	//~ if(seePlayer)
	//~ {
		
	//~ }
	//~ else
	//~ {
		
	//~ }
}



bool NPC::patrolingSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
{
	if(resetCurrentLogic)
	{
		resetCurrentLogic = false;
		return true;	//в экстренном случае снимем текущую задачу
	}
	
	//~ printMessage("\nPATROLING")
	//~ printVec3(position)
	//~ printVec3(targetPoint)
	
	//~ restartIntervalTimer += tick;
	
	//~ if(restartIntervalTimer > 3000)
	//~ {
		//~ restartIntervalTimer = 0;
		//~ printMessage("Watchdog patrol")
		//~ DEBUG_INFO
		//~ return true;
	//~ }
	
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
		auto lab = dynamic_pointer_cast<Laboratory> (level);			//Laboratory* lab = dynamic_cast<Laboratory*>(level);
		
		if(step > 300 && lab != nullptr && lab->sound4done)
		{
			step=0;
			sound_NPCwalking->playSound( [](){}, [](){} );
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



bool NPC::pursuitSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
{
	restartIntervalTimer+= tick;
	
	if( restartIntervalTimer > 500 )
	{
		restartIntervalTimer = 0;
	}
	
	glm::vec3 npcToPlayerVec = glm::normalize( *CameraPositionPtr - getPosition() );	// вектор на игрока
	glm::vec3 npcForwardVec = m3VectorsForwardLeftUp[0];	// впередсмотрящий вектор монстра
	float dot = glm::dot(npcForwardVec, npcToPlayerVec  );
	
	if(dot > std::cos(FOV/2*3.14/180) ) // пока игрок в секторе обзора
	{
		std::shared_ptr<MicroWorld> caster = traceRay( npcToPlayerVec );
		if(caster)			
		{
			if(caster->MicroWorldName == "Camera")	// и если виден именно игрок!
			{
				glm::vec3 playerPos = glm::vec3( caster->getPosition().x, caster->getPosition().y, 0.1);
				
				// добавить интервал по времени, а то перезапуск происходит в каждом кадре
				//if(playerPos != targetPoint && restartIntervalTimer > 490-tick )	// если игрок сдвинулся  в течении примерно полусекунды
				//{
					//targetPoint = glm::vec3( caster->getPosition().x, caster->getPosition().y, 0.1);	// обновим целевую точку
					
					//focuseToSpecifiedPoint( targetPoint, 100, std::bind(&NPC::rotating_start, this), std::bind(&NPC::rotating_end, this)	);	// перезапустим цикл преследования
				//	return true; 	// закончим текущий цикл
				//}
				//else 										// если игрок на месте
				//{
					glm::vec3 direction = targetPoint - position;
					
					if( alive && glm::length( direction ) > 2.0f )	// покуда расстояние до цели больше пороговых 0.5 м
					{
						rotation = getHorizontalQuaternionFocusedToPoint( playerPos );
						targetPoint = glm::vec3( caster->getPosition().x, caster->getPosition().y, 0.1);	// обновим целевую точку
						
						moveInDirection( mainCollisionEntity, direction, idleWalkingSpeed * float(tick)/1000.0f );
						moveInDirection( mainCollisionEntity, -glm::vec3(0,0,1), idleWalkingSpeed * float(tick)/1000.0f );
						attacking = false;
						return false;
					}
					else 		// достигнув цели, обновим параметры состояния
					{
						attacking = true;	
						return true;
					}
				//}
			}
			else 		// если виден не игрок
			{
				seePlayer = false;
				return true;		// заканчиваем цикл преследования, т.к. видим НЕ игрока
			}
		}
		else
		{
			return false;
		}
	}
	else 		// если игрок вне сектора обзора
	{
		seePlayer = false;
		return true;		// тоже заканчиваем цикл преследования, т.к. не видим игрока
	}
	
	
	
}


void NPC::stepSound()
{
	sound_NPCwalking->playSound( [](){}, [](){} );
}

void NPC::patrolling_start()
{
	printMessage("start patrolling")
	
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

void NPC::patrolling_end()
{
	printMessage("end patrolling")

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
				orcAnimation.blendingTimeIn = 500;
				
				orcAnimation.end_clb = std::bind( &NPC::foo, this);	// разрешим обновлять логику спустя цикл анимации
			
			components[mainVisualEntity]->setAnimation(0, orcAnimation, orcAnimation	);	
		}
		else
		{
			logicChangeable = true;	// или сразу
		}
			
		
		//seePlayer = false; // XXX этот флаг надо переименовать
		//~ glm::vec3 npcToPlayerVec = glm::normalize( *CameraPositionPtr - getPosition() );	// вектор на игрока
		//~ glm::vec3 npcForwardVec = m3VectorsForwardLeftUp[0];	// впередсмотрящий вектор монстра
		//~ float dot = glm::dot(npcForwardVec, npcToPlayerVec  );
		
		//~ if(dot > std::cos(FOV/2*3.14/180) )
		//~ {
			//~ std::shared_ptr<MicroWorld> caster = traceRay( npcToPlayerVec );
			//~ if(caster && caster->MicroWorldName == "Camera")
			//~ {
				//~ printVarComment("Casted combo:", caster->MicroWorldName )
				//~ sound_SeePlayer->playSound();
				//~ ////seePlayer = true;
				
				//~ targetPoint = glm::vec3( caster->getPosition().x, caster->getPosition().y, 0.1);	// getRandomPointInRadius( glm::vec3(7, 8, 0), 5.0f);
				//~ focuseToSpecifiedPoint( targetPoint, 100, std::bind(&NPC::rotating_start, this), std::bind(&NPC::rotating_end, this)	);
			//~ }
		//~ }
	}

}

void NPC::foo()
{
	printMessage("END CALLBACK FOR ANIMATION")
	logicChangeable = true;
	hitSound->playSound( [](){}, [](){} );
}

glm::vec3 NPC::getRandomPointInRadius( const glm::vec3& point, float radius )
{
	int start = -100 * radius;
	int end = 100 * radius;

	float x = (rand() % (end - start + 1) + start) / 100;
	float y = (rand() % (end - start + 1) + start) / 100;

	glm::vec3 randomPoint = glm::vec3(x, y, point.z);

	return point + randomPoint;
}


void NPC::takeDamage(float damage, glm::vec3 source)		// добавить в аргументы ссылку на класс, от кого пришел урон. Для фильтрации френдлифаера
{
	if(HP-damage > 0 )// && alive)
	{
		sound_Pain->playSound( [](){}, [](){} );
		HP -= damage;
		//printVarComment("Take damage, estimated hp: ", HP)
		
		targetPoint = glm::vec3( source.x, source.y, 0.1);	// целью сделаем точку возникновения угрозы
		focuseToSpecifiedPoint( targetPoint, 400, std::bind(&NPC::empty_clb_start, this), std::bind(&NPC::empty_clb_end, this)	);
		
		resetCurrentLogic = true;
	}
	else
	{
		if(alive)
		{
			// анимация смерти, оканчивающаяся на последнем кадре
			animStateConfig orcDeathAnimation;
				orcDeathAnimation.mixCoeff = 0.0f;
				orcDeathAnimation.mask = {};
				orcDeathAnimation.animName = dieAnimationName;
				orcDeathAnimation.repeatCount = -2; // -1 значит бесконечный повтор, -2 значит однократное выполнение и зависание на последнем кадре
				orcDeathAnimation.blendingTimeIn = 500;
				orcDeathAnimation.start_clb = std::bind(&NPC::death_start, this);
				orcDeathAnimation.end_clb = std::bind(&NPC::death_end, this);
			components[mainVisualEntity]->setAnimation(0, orcDeathAnimation, orcDeathAnimation	);	
			
			alive = false;
		}
	}
}



void NPC::death_start()
{
	sound_Death->playSound( [](){}, [](){} );
	
	components[mainCollisionEntity]->disableCollision();
	
	std::shared_ptr<Door_01> door_2 = dynamic_pointer_cast<Door_01>( level->getActor("Door_02") );
	if(door_2)
	{
		door_2->locked = false;
	}
	
	std::shared_ptr<Door_01> door_3 = dynamic_pointer_cast<Door_01>( level->getActor("Door_03") );
	if(door_3)
	{
		door_3->locked = false;
	}
	
}
void NPC::death_end()
{
	//printMessage("YOU KILLED NPC!")
}










bool NPC::bDeathSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
{
	
	// на основе этой функции можно сделать обертку для проигрыша анимаций, с назначением колбеков на начало и конец
	int duration = 5000;
	
	if(components.count(mainVisualEntity) != 0)
	{
		duration = components[mainVisualEntity]->entitySkeletonPtr->sklAnimations[dieAnimationName]->getDuration();	//.count( newAnim.animName )
		printVarComment("Dead sequence duration ", duration)
	}
	
	
	if(iDeathTimer > duration)
	{
		iDeathTimer = 0;
		return true;
	}
	
	//~ if(iDeathTimer > 3300)
	//~ {
		//~ components["Dynamite"]->visible = true;
	//~ }
	
	iDeathTimer+= tick;
	
	return false;
}

//////////////

//~ printMessage("NPC logic")
	
	//~ if( glfwGetKey(engineWindow, GLFW_KEY_1) && !keys[0] )
	//~ {	keys[0] = true;
		
		//~ focuseToSpecifiedPoint(  position + glm::vec3(10,10,0), 1000, std::bind(&NPC::empty_clb_start, this), std::bind(&NPC::empty_clb_end, this)	);	
		
	//~ }
	//~ if( !glfwGetKey(engineWindow, GLFW_KEY_1) ) {keys[0] = false;}
	
	
	
	//~ if( glfwGetKey(engineWindow, GLFW_KEY_2) && !keys[1] )
	//~ {	keys[1] = true;
		
		//~ focuseToSpecifiedPoint( position + glm::vec3(-10,10,0), 1000, std::bind(&NPC::empty_clb_start, this), std::bind(&NPC::empty_clb_end, this)	);
	//~ }
	//~ if( !glfwGetKey(engineWindow, GLFW_KEY_2) ) {keys[1] = false;}
	
	
	
	//~ if( glfwGetKey(engineWindow, GLFW_KEY_3) && !keys[2] )
	//~ {	keys[2] = true;
		
		//~ focuseToSpecifiedPoint(  position + glm::vec3(-10,-10,0), 1000, std::bind(&NPC::empty_clb_start, this), std::bind(&NPC::empty_clb_end, this)	);
		
	//~ }
	//~ if( !glfwGetKey(engineWindow, GLFW_KEY_3) ) {keys[2] = false;}
	
	
	
	
	//~ if( glfwGetKey(engineWindow, GLFW_KEY_4) && !keys[3] )
	//~ {	keys[3] = true;
		
		//~ focuseToSpecifiedPoint( position + glm::vec3(10,-10,0), 1000, std::bind(&NPC::empty_clb_start, this), std::bind(&NPC::empty_clb_end, this)	);
		
	//~ }
	//~ if( !glfwGetKey(engineWindow, GLFW_KEY_4) ) {keys[3] = false;}
	
		
	//~ printMessage("NPC logic")
	
	//~ if(dot < 0.65 && seePlayer) 
	//~ {
		//~ seePlayer = false;
		
		//~ animStateConfig npcAnimation;
			//~ npcAnimation.mixCoeff = 0.0f;
			//~ npcAnimation.mask = {};
			//~ npcAnimation.animName = idleAnimationName;
			//~ npcAnimation.repeatCount = -1;
			//~ npcAnimation.blendingTimeIn = 500;
		
		//~ components[mainEntity]->setAnimation(0, npcAnimation, npcAnimation	);	
		
	//~ }
	
	
	
	

	//~ printMessage("PATROLING")
	//~ printVec3(position)
	//~ printVec3(targetPoint)
	
	//~ glm::vec3 direction = targetPoint - position;
	
	//~ if( alive && glm::length( direction ) > 0.5f )	// покуда расстояние до цели больше пороговых 0.5 м
	//~ {
		//~ moveInDirection( mainCollisionEntity, direction, idleWalkingSpeed * float(tick)/1000.0f );
		//~ moveInDirection( mainCollisionEntity, -glm::vec3(0,0,1), idleWalkingSpeed * float(tick)/1000.0f );

		//~ glm::vec3 npcToPlayerVec = glm::normalize( *CameraPositionPtr - getPosition() );	// вектор на игрока
		//~ glm::vec3 npcForwardVec = m3VectorsForwardLeftUp[0];	// впередсмотрящий вектор монстра
		//~ float dot = glm::dot(npcForwardVec, npcToPlayerVec  );
		//~ //printVariable(dot)
		//~ if(dot > std::cos(FOV/2*3.14/180) )// && !seePlayer)
		//~ {
			//~ std::shared_ptr<MicroWorld> caster = traceRay( npcToPlayerVec );
			//~ if(caster && caster->MicroWorldName == "Camera")
			//~ {
				//~ //printVarComment("Casted combo:", caster->MicroWorldName )
				//~ sound_SeePlayer->playSound();
				//~ //seePlayer = true;
				//~ targetPoint = glm::vec3( caster->getPosition().x, caster->getPosition().y, 0.1);	// getRandomPointInRadius( glm::vec3(7, 8, 0), 5.0f);
			//~ }
		//~ }
		
		
		//~ // position += glm::normalize(direction) *;	// * glm::normalize( targetPoint - position );	// добавим смещения
		
		
		//~ static int step = 0;
		//~ Laboratory* lab = dynamic_cast<Laboratory*>(level);
		
		//~ if(step > 300 && lab != nullptr && lab->sound4done)
		//~ {
			//~ step=0;
			//~ sound_NPCwalking->playSound();
		//~ }
		//~ step+=tick;
		
		
		
		//~ return false;
	//~ }
	//~ else
	//~ {
		//~ return true;
	//~ }
	
	
	// check if see player
	
	
	//~ return false;
