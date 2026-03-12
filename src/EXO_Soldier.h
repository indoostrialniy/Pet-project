#pragma once
#ifndef EXO_SOLDIER_H
#define EXO_SOLDIER_H

#include "NPC.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "global.h"
#include <memory>
#include "SoundAsset.h"

class EXO_Soldier : public NPC
{
public:
	EXO_Soldier() {};
	EXO_Soldier( glm::vec3 worldLoc, glm::quat worldRot, std::string n );


	void logic() override; 	
	
	bool triggered = false;
	
	bool armed = false;
	
	
	void rotating_start();
	void rotating_end();
	
	void foo();
	
	void meeting_start();
	void meeting_end();
	
	bool patrolingSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	
	void evac_done_start();
	void evac_done_end();
	
	std::shared_ptr<SoundAsset> sound_1 = SoundAssets.getAsset("naparnik.wav");
	std::shared_ptr<SoundAsset> sound_2 = SoundAssets.getAsset("adios.wav");


	
	//~ bool keys[4]{false, false, false, false};
};


#endif
