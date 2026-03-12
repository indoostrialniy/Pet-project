#include "Turniket.h"
#include "CollisionDescriptor.h"

#include "Level.h"

Turniket_01::Turniket_01( glm::vec3 worldLoc, glm::quat worldRot, std::string n )
{
	MicroWorldName = n;
	
	position = worldLoc;		//назначим координаты
	rotation = worldRot;
	
	CollisionDescriptor emptyCollision;

	addEntity( "SM_Turniket_01", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Turniket_01", emptyCollision, "MESH", { "M_Metal_Rust_001_0_5k_x1",  "M_Metal_Rust_002_0_5k_x1", },  "none",  "forward" );
	addEntity( "SM_Turniket_Balka_01", glm::vec3(0.0172, -0.0, 0.8808), glm::quat(0.2706, -0.6533, 0.6533, 0.2706),  "SM_Turniket_Balka_01", emptyCollision, "MESH", { "M_Metal_Rust_002_0_5k_x1", },  "none",  "forward" );

	treshold = 2.0f;

	movePartSoundPtr = 	SoundAssets.getAsset("zvuk_rgavoy_metallicheskoy_dveri.wav");

	lidProcConfig balkaMovingPart;										// создадим структуру с конфигурацией двигающегося компонента
		balkaMovingPart.name = "SM_Turniket_Balka_01";							// указываем конкретны компонент
		balkaMovingPart.keyCode = GLFW_KEY_E;								// реагирует на клавишу T
		balkaMovingPart.transformationType = MMP_ROTATABLE;				// вращается
		balkaMovingPart.defaultQuat = glm::quat(0.2706, -0.6533, 0.6533, 0.2706);
		balkaMovingPart.targetQuat = glm::quat(-0.099, 0.239, 0.892, 0.37);		//
		balkaMovingPart.timeForRotation = 1000;
		
		balkaMovingPart.start_callback 	= [&](){ movePartSoundPtr->playSound( [](){}, [](){} ); };
		balkaMovingPart.end_callback	 = [&](){ movePartSoundPtr->stopSound(); };	
	
	mainMovingPartsMassive[MMP_ZERO]  = std::move(balkaMovingPart); 		// переместили в массив конфигураций компонентов
	
	//~ lidProcConfig balka2MovingPart;										// создадим структуру с конфигурацией двигающегося компонента
		//~ balka2MovingPart.name = "SM_Turniket_Balka_01";							// указываем конкретны компонент
		//~ balka2MovingPart.keyCode = GLFW_KEY_R;								// реагирует на клавишу T
		//~ balka2MovingPart.transformationType = MMP_ROTATABLE;				// вращается
		//~ balka2MovingPart.targetQuat = glm::quat(0.3827, 0.0, 0.9239, 0.0);
		//~ balka2MovingPart.defaultQuat = glm::quat(0.191, 0.8, 0.462, 0.331);		//
		//~ balka2MovingPart.timeForRotation = 1000;
	
	//~ mainMovingPartsMassive[MMP_FIRST]  = std::move(balka2MovingPart); 		// переместили в массив конфигураций компонентов
	

	//~ movePartBuffer.loadFromFile("./datas/audio/zvuk_skripa_bolshih_vorot_kotorie_pitayutsya_otkryt.wav");
	//~ movePartSound = sf::Sound(movePartBuffer);
	status = true;
}


void Turniket_01::logic()
{
	auto player = level->getActor("Camera");
	
	if(player)
	{
		glm::vec3 npcToPlayerVec =  player->getPosition() - getPosition() ;	// вектор на игрока
		glm::vec3 npcForwardVec = m3VectorsForwardLeftUp[0];	// впередсмотрящий вектор 
		float dot = glm::dot(npcForwardVec, npcToPlayerVec  );
		//~ printVariable(dot)
		
		float dist = glm::length( npcToPlayerVec );
		//~ printVariable(dist)
		//~ printVariable(triggered)
		
		if( dist < 1.7 && (dot > std::cos(70/2*3.14/180) || dot < std::cos(145/2*3.14/180) ) )
		{
			if(dot > 0 && !front && !triggered)
			{
				triggered = true;
				launchMainMovingPartMovement(MMP_ZERO, std::bind(&Turniket_01::turningSound_start, this), std::bind(&Turniket_01::turningSound_end, this) );
				front = true;
			}
			
			if(dot < 0 && front && !triggered)
			{
				triggered = true;
				launchMainMovingPartMovement(MMP_ZERO, std::bind(&Turniket_01::turningSound_start, this), std::bind(&Turniket_01::turningSound_end, this) );
				front = false;
			}
		}
		
		
		
		
		//~ if(dot > 0 )	//std::cos(80/2*3.14/180) ) // пока игрок в секторе обзора
		//~ {
			//~ if( dist < 1.64f && !triggered)
			//~ {
				//~ triggered = true;
				//~ // printMessage(" turn from front to back")
				//~ launchMainMovingPartMovement(MMP_ZERO, std::bind(&Turniket_01::turningSound_start, this), std::bind(&Turniket_01::turningSound_end, this) );
			//~ }
			//~ // printMessage("front")
		//~ }
		//~ else
		//~ {
			//~ if( dist < 1.64f && !triggered)
			//~ {
				//~ triggered = true;
				//~ // printMessage(" turn from back to front")
				//~ launchMainMovingPartMovement(MMP_ZERO, std::bind(&Turniket_01::turningSound_start, this), std::bind(&Turniket_01::turningSound_end, this) );
			//~ }
		//~ }
		
		//~ if(dist > 1.65) {triggered = false;}
	}
}

void Turniket_01::turningSound_start()
{
	movePartSoundPtr->playSound( [](){}, [](){} );
}

void Turniket_01::turningSound_end()
{
	movePartSoundPtr->stopSound();
	triggered = false;
	
	if(!lootingHintGiven)
	{
		lootingHintGiven = true;
		lootingSound->playSound( [](){}, [](){} );
	}
}
