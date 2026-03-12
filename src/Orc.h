#pragma once
#ifndef ORC_H
#define ORC_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

#include "NPC.h"
#include "SoundAsset.h"
#include "global.h"

class Orc : public NPC
{
public:
	Orc() {};
	Orc( glm::vec3 worldLoc, glm::quat worldRot, std::string n );

	void logic() override; 			// переназначение логики класса
	
	bool spotted = false;
	bool meeting = false;

	
	bool bOrcArrived = false;		// свидетельство эффектного появления орка

	bool orcPatrolingSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	void orcPatrolingSequence_end();

	void orcRotationIdle_start();
	void orcRotationIdle_end();
	
	void monsterArrivingSound_start();
	void monsterArrivingSound_end();
	
	void playerFocuseAtOrk_start();
	void playerFocuseAtOrk_end();
	
	
	void monsterArriving_start();
	bool monsterArrivingSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	void monsterArriving_end();
	
	
	void orcPursuiting_start();
	int iOrcPursuitingWatchDog = 0;
	bool orcPursuitingSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	void orcPursuiting_end();
	
	void hitAnimation_end();
	
	std::shared_ptr<SoundAsset> screamer2SoundPtr 	= SoundAssets.getAsset("monstr-ryichit-32517.wav"); 
	
	std::shared_ptr<SoundAsset> screamSoundPtr 	= SoundAssets.getAsset("krik-monstra.wav");
	std::shared_ptr<SoundAsset> jumpSoundPtr 	= SoundAssets.getAsset("blizkiy-gromkiy-hlyupayuschiy-pryijok-v-vodu.wav");

	void orcRotation_start();
	void orcRotation_end();
	//~ bool keys[4]{false, false, false, false};
};


#endif
