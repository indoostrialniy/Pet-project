#pragma once
#ifndef TRANSPORTER_H
#define TRANSPORTER_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <SFML/Audio.hpp>
#include <memory>

#include "MovingPartsController.h"
#include "SoundAsset.h"
#include "global.h"

class Transporter : public MovingPartsController
{
public:	
	Transporter( glm::vec3 worldLoc, glm::quat worldRot, std::string n);

	bool triggered = false;
	
	bool preparing = false;
	
	bool processing = false;
	
	std::shared_ptr<SoundAsset> sound_Transporting = SoundAssets.getAsset("vstryahivanie-odejdyi-36071.wav");

	
	
	//~ bool active = true;

	//~ bool goToStartPosition(void* ptr, int var);

	//~ int goToStartPositionTimer = 0;
	
	//~ bool goToStart = false;
	int timer = 0;

	bool flag_E = false;	//mainMovingPartsMassiveStatuses[2];		// для обработки клавиши E
	
	glm::vec3 startPos = glm::vec3(0,0,0);
	
	///bool elevatorDoorsOpen = false;//mainMovingPartsMassiveStatuses[0];
	
	//~ std::shared_ptr<SoundAsset> elevatorRadioSoundPtr = SoundAssets.getAsset("mutant_generic_A_3.wav");

	//~ int goToGatesTimer = 0;
	
	//~ int elevatorOpenDoorTimer = 0;
	
	//~ static bool goToGates(void* ptr, int var);
	
	//~ static bool openGates(void* ptr, int var);
	
	void logic() override;
	//~ void update();
	void startTransporting();
	void endTransporting();

};


#endif
