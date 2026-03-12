#pragma once
#ifndef NPC_H
#define NPC_H

#include "AI_Controller.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "global.h"
#include <memory>
#include "SoundAsset.h"

class NPC : public AI_Controller
{
public:
	NPC() {};
	NPC( glm::vec3 worldLoc, glm::quat worldRot, std::string n );


	void logic() override; 
	
	bool flag1 = false;
	
	
	
	

	
	std::string idleAnimationName 		= "none"	;
	std::string walkAnimationName 		= "none"	;
	std::string attackAnimationName 	= "none"	;
	std::string dieAnimationName 		= "none"	;
	
	
	int HP = 100;
	
	bool alive = true;
	
	bool logicChangeable = true;
	
	void stepSound();
	
	glm::vec3 targetPoint = glm::vec3(0,0,0);
	
	
	glm::vec3 getRandomPointInRadius( const glm::vec3& point, float radius );

	
	float idleWalkingSpeed = 2.0f; // сдвиг N см за шаг
	
	float alarmWalkingSpeed = 6.0f; // сдвиг 2N см за шаг
	
	bool attacking = false;
	
	void rotating_start();
	void rotating_end();


	float FOV = 140.0;	// 90 градусов
	
	bool pursuitSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);

	
	bool patrolingSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	void patrolling_start();
	void patrolling_end();


	bool keys[4]{false, false, false, false};

	void empty_clb_start();
	void empty_clb_end();

	void foo();
	
	bool resetCurrentLogic = false;
	
	int restartIntervalTimer = 0;
	
	enum state{ NPC_IDLE, NPC_ALARMED, NPC_ANGRY};	// 0 - в покое, 1 - встревожен, 2 - тревога
	
	bool seePlayer = false;							// заметил ли игрока
	
	std::shared_ptr<SoundAsset> sound_SeePlayer = SoundAssets.getAsset("monstr-odinochnyii-korotkii-hrustyaschii.wav");
	
	std::shared_ptr<SoundAsset> sound_Pain = SoundAssets.getAsset("zvuk-boli.wav");
	
	std::shared_ptr<SoundAsset> sound_Death = SoundAssets.getAsset("orcDeath.wav");
	
	std::shared_ptr<SoundAsset> sound_NPCwalking = SoundAssets.getAsset("orcWalkingSound.wav");

	std::shared_ptr<SoundAsset> hitSound 	= SoundAssets.getAsset("zvuk-udara-nojom.wav");

	//~ std::shared_ptr<SoundAsset> sound_SeePlayer = SoundAssets.getAsset("monstr-odinochnyii-korotkii-hrustyaschii.wav");
	//~ std::shared_ptr<SoundAsset> sound_SeePlayer = SoundAssets.getAsset("monstr-odinochnyii-korotkii-hrustyaschii.wav");
	
	

	
	void takeDamage( float damage, glm::vec3 source );
	
	int iDeathTimer = 0;
	bool bDeathSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	void death_start();
	void death_end();
	
};


#endif
