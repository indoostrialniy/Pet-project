#pragma once
#ifndef GATE_01_H
#define GATE_01_H

#include "MovingPartsController.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include "SoundAsset.h"
#include "global.h"

class Gate_01 : public MovingPartsController
{
public:	
	Gate_01( glm::vec3 worldLoc, glm::quat worldRot, std::string n);
	
	void logic() override; // типовая логика из класса MovingPartsController будет заменена методом из этого класса
	
	
	
	bool bGateActivated = false;
	
	bool wheel_end(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);


	
	//~ gateOpenSoundPtr 		
	//~ whellTurningSoundPtr	
	//~ gateFailureSoundPtr 
		
	// звуки
	std::shared_ptr<SoundAsset> gateOpenSoundPtr 		= SoundAssets.getAsset("zvuk_skripa_bolshih_vorot_kotorie_pitayutsya_otkryt.wav");
	std::shared_ptr<SoundAsset> whellTurningSoundPtr 	= SoundAssets.getAsset("zvuk_rgavoy_metallicheskoy_dveri.wav");
	std::shared_ptr<SoundAsset> gateFailureSoundPtr 	= SoundAssets.getAsset("gate_failure_open.wav");

	void approachingForOpeningGate_start();
	void approachingForOpeningGate_end();

	void openingGate_start();
	void openingGate_end();
	
	void gateFailureSound_start();
	void gateFailureSound_end();

	//~ bool approachingForOpeningGate_start(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	//~ bool approachingForOpeningGate_end(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);


	

};



#endif
