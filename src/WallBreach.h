#pragma once
#ifndef WALL_BREACH_H
#define WALL_BREACH_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <SFML/Audio.hpp>
#include <memory>

#include "MovingPartsController.h"
#include "SoundAsset.h"
#include "global.h"

class WallBreach_01 : public MovingPartsController
{
public:	
	WallBreach_01( glm::vec3 worldLoc, glm::quat worldRot, std::string n);
		
	
	std::shared_ptr<SoundAsset> sound_IdeaForBlowing = SoundAssets.getAsset("IdeaForBlowing.wav");
	
	std::shared_ptr<SoundAsset> sound_Bomb_timer_launched = SoundAssets.getAsset("BombTimerLaunched.wav");
	
	std::shared_ptr<SoundAsset> sound_Bomb_planted = SoundAssets.getAsset("BombPlanted.wav");
	
		
	bool bWallBreachInitialized = false;		// флаг активизации пролома
	
	bool bWallBreachIdeaGiven = false;			// флаг высказывания идеи подорвать стену
	bool bWallBreachActorIsNear = false;		// флаг того, что игрок подошел к месту пролома
	
	
	
	bool dynamiteInstalled = false;
	bool sound_Bomb_timer_launcheded = false;
	
	int iDynamiteInstallCounter = 0;
	
	int iDynamiteTimerCounter = 0;
	
	
	void logic() override; // типовая логика из класса MovingPartsController будет заменена методом из этого класса
	
	void wallBreachFocuse_start();
	void wallBreachFocuse_end();

	void wallBreachApproaching_start();
	void wallBreachApproaching_end();

	bool dynamiteInstallSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	void dynamiteInstall_Start();
	void dynamiteInstall_End();
	
	
	bool bombLaunchSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	void bombLaunch_Start();
	void bombLaunch_End();
	
	
	
	int goToBreachTimer = 0;
	bool goToBreach(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	

	bool focuse_end(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);

};


#endif
