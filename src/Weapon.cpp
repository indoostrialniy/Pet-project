#include "Weapon.h"

#include "Collision.h"

#include <functional>

#include "macros.h"
#include "Level.h"
#include "NPC.h"

#include "Skeleton.h"
#include "SkeletonAnimation.h"

#include "SoundAsset.h"

#include "DynamicLight.h"
#include "Entity.h"
#include "AnimationController.h"

Weapon::Weapon() 
{	
	
}



glm::vec4 Weapon::getSocketPosition() 
{ 
	return socketPosition; 
}


glm::vec4 Weapon::getSocketRotationXYZW()
{ 
	return socketRotationXYZW; 
}


glm::vec4 Weapon::getSocketPositionPlayer()
{
	return socketPositionPlayer; 
}

glm::vec4 Weapon::getSocketRotationPlayerXYZW()
{
	return socketRotationPlayerXYZW; 
}


bool Weapon::bWeaponShot()												// описание всех действий для стрельбы
{
	if(bWeaponShotReady)												// только если готов к выстрелу
	{
		//printMessage("Weapon shot!!")
		
		// в любом случае запустим движение курка, чьи колбеки временно заморозят возможность стрелять во избежание залипания клавиш
		launchMainMovingPartMovement( MMP_FIRST, std::bind(&Weapon::weaponShotTrigger_start, this), std::bind(&Weapon::weaponShotTrigger_end, this) ); 	
		
		if( currentAmmoCount > 0 )										// если патронов достаточно...
		{
			launchMainMovingPartMovement( MMP_ZERO, std::bind(&Weapon::weaponShotShutter_start, this), std::bind(&Weapon::weaponShotShutter_end, this)  ); 				// запуск движения затворной рамы
			
			
			
			
			//~ //IF_DYNAMIC_CAST(Dynamic_Light*, light, MicroWorldPersonalLightSourcePtr)
			//~ //{
			//~ //	light->launchMainMovingPartMovement(nullptr, 0);	// запуск вспышки
			//~ //}
			
			//MicroWorldPersonalLightSourcePtr->visible = true; 
			
			if(currentAmmoCount == 1)	// последний патрон
			{
				mainMovingPartsMassive[MMP_ZERO].transformationType = MMP_MOVABLE;
			}
			
			return true;
		} 
		else
		{
			printVarComment("No ammo in weapon:", MicroWorldName)
			
			return false;
		}
	}
	else
	{
		return false;
	}
	
	
}

void Weapon::weaponShotTrigger_start()									// колбек начала
{
	//~ printMessage("bWeaponShotTrigger_start")
	
	bWeaponShotReady = false;							// временно заблокируем способность стрелять от дальнейших нажатий
	
	if( CHECK_PTR(weapponNoAmmoSoundPtr) )
	{
		weapponNoAmmoSoundPtr->playSound( [](){}, [](){} );
	}
}
void Weapon::weaponShotTrigger_end()									// колбек конца
{
	//~ printMessage("bWeaponShotTrigger_end")

	bWeaponShotReady = true;							// по завершении нажатия курка возобновим возможность стрелять
}

void Weapon::weaponShotShutter_start()									// начало выстрела
{
	//~ printMessage("weaponShotShutter_start")
		
	if( CHECK_PTR(weaponShotSoundPtr) )
	{
		weaponShotSoundPtr->playSound( [](){}, [](){} );				// громыхнем в начале
	}
	
	if(!bSparkSpawned)	// однократно заспауним и привяжем источник света
	{
		bSparkSpawned = true;
		std::string name = MicroWorldName+"_light";
		level->spawnActor<DynamicLight_01>( name, getPosition(), getRotation() );  // заспауним сет динамического света
		auto light = level->getActor(name);
		if( CHECK_PTR(light) )
		{
			light->attachTo( this, "Pistol", "none", glm::vec4(0.11, 0, 0.06, 1), glm::vec4(0,0,0,1) ); 
		}
		
		
	}
	
	if( childMicroWorld!=nullptr && childMicroWorld->hasTag("Light") )
	{
		childMicroWorld->visible = true;
	}
	
	//vec4(0.5317, -0.087, -0.089, 1), vec4(-0.272, 0.066, 0.156, 0.947));

		//~ MicroWorldPersonalLightSourcePtr->MicroWorldAttachTo(this, "Pistol", "none", glm::vec4(0.11, 0, 0.06, 1), glm::vec4(0,0,0,1)); //и привяжем его
	
}
void Weapon::weaponShotShutter_end()									// конец выстрела
{
	//~ printMessage("\n\n\nweaponShotShutter_end")
	
	currentAmmoCount--;									// в конце уменьшим число патронов

	if( childMicroWorld!=nullptr && childMicroWorld->hasTag("Light") )
	{
		childMicroWorld->visible = false;
	}
	
	std::shared_ptr<MicroWorld> caster = traceRay( m3VectorsForwardLeftUp[0] );		// определим, в кого попали
	
	if(caster)
	{
		//~ printVarComment("Shot: ", caster->MicroWorldName)
		
		std::shared_ptr<NPC> monster = dynamic_pointer_cast<NPC>(caster);
		if(monster)
		{
			monster->takeDamage(damage, position);
		}
	}
	

}






bool Weapon::bWeaponReload()
{
	//~ printMessage("\nReloading!")
	
	//~ mainMovingPartsMassive[MMP_ZERO].timeForMoving = 2000;
	//~ launchMainMovingPartMovement( MMP_ZERO, std::bind(&Weapon::weaponReload_start, this), std::bind(&Weapon::weaponReload_end, this)  ); 	
	
	BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;		
	std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function = std::bind( &Weapon::bWeaponReloadSequence, this, datas);	// главная вращающая функция
	auto start = std::bind( &Weapon::weaponReload_start, this);		// колбек начала
	auto end = std::bind( &Weapon::weaponReload_end, this);		// колбек конца
	Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
	
	return true;
}

bool Weapon::bWeaponReloadSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
{
	int duration = 5000;
	
	auto camera = level->getActor("Camera");
	if(camera && camera->components.count("Player_Hands_MESH") != 0)
	{
		duration = camera->components["Player_Hands_MESH"]->entitySkeletonPtr->sklAnimations[reloadPlayerAnimation]->getDuration();
	}
	
	if(iWeaponReloadTimer > duration)
	{
		iWeaponReloadTimer = 0;
		return true;
	}
	
	if(iWeaponReloadTimer > 1300)
	{
		//some action
	}
	
	iWeaponReloadTimer+= tick;
	
	return false;
}
void Weapon::weaponReload_start()
{
	animStateConfig handsAnimation, noSecondAnimation;
		handsAnimation.mixCoeff = 0.0f;
		handsAnimation.mask = {};
		handsAnimation.animName = reloadPlayerAnimation;
		handsAnimation.repeatCount = 1;
		handsAnimation.blendingTimeIn = 500;
	
	auto camera = level->getActor("Camera");
	if(camera) camera->components["Player_Hands_MESH"]->setAnimation(0, handsAnimation, noSecondAnimation	);
	
	weaponReloadSoundPtr->playSound( [](){}, [](){} );
}
void Weapon::weaponReload_end()
{
	currentAmmoCount = ammoMaxCap;
	
	launchMainMovingPartMovement( MMP_ZERO, [](){}, [](){} );	// сдвинем затвор в исходное
	mainMovingPartsMassive[MMP_ZERO].transformationType = MMP_MOVABLE_REVERSE;
}





bool Weapon::bWeaponAim( bool status )							// описание всех действий для прицеливания
{
	if( CHECK_PTR(parentMicroWorld) && parentMicroWorld->components.count("Player_Hands_MESH") != 0 )
	{
		//~ printMessage("Set player aim anim")
		
		//~ if( flag_ )
		//~ {
			//~ parentMicroWorld->components["Player_Hands_MESH"]->setAnimation(0, { 0.0f, {}, aimPlayerAnimation, 1, 120 }, { 0.0f, {}, aimPlayerAnimation, -1, 120 } );	
		//~ }
		//~ else
		{
			animStateConfig idleAnimation;
				idleAnimation.mixCoeff = 0.0f;
				idleAnimation.mask = {};
				idleAnimation.animName = aimPlayerAnimation;
				idleAnimation.repeatCount = -1;
				idleAnimation.blendingTimeIn = 250;
			
			
			animStateConfig aimAnimation;
				aimAnimation.mixCoeff = 0.0f;
				aimAnimation.mask = {};
				aimAnimation.animName = idlePlayerAnimation;
				aimAnimation.repeatCount = -1;
				aimAnimation.blendingTimeIn = 250;
			
			auto camera = level->getActor("Camera");
			
			if(camera) 
			{
				if(!status) // целится
				{
					camera->components["Player_Hands_MESH"]->setAnimation(0, aimAnimation, aimAnimation	);
				}
				else 	// не целится
				{
					camera->components["Player_Hands_MESH"]->setAnimation(0, idleAnimation, idleAnimation	);
				}
			}
				
			
			//parentMicroWorld->components["Player_Hands_MESH"]->setAnimation(0, { 0.0f, {}, idlePlayerAnimation, 1, 120 }, { 0.0f, {}, idlePlayerAnimation, -1, 120 } );	
		}
		

		//std::string idlePlayerAnimation = "Hands_F";
		//std::string aimPlayerAnimation = "Hands_F";
		//std::string takingPlayerAnimation = "Hands_F";
	}
	
	//flag_ = !flag_;
	return true;
}



