#pragma once
#ifndef DOOR_01_H
#define DOOR_01_H

#include "MovingPartsController.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include "SoundAsset.h"
#include "global.h"

class Door_01 : public MovingPartsController
{
public:	
	Door_01( glm::vec3 worldLoc, glm::quat worldRot, std::string n);
	
	std::shared_ptr<SoundAsset> sound_doorLocked = SoundAssets.getAsset("doorLockerIdea.wav");
	std::shared_ptr<SoundAsset> sound_doorLocked2 = SoundAssets.getAsset("skrip-dvernoy-ruchki.wav");
	
	std::shared_ptr<SoundAsset> sound_doorOpening = SoundAssets.getAsset("derevyannaya-dver-otkryivaetsya-so-skripom-37573.wav");

	
	void logic() override; // типовая логика из класса MovingPartsController будет заменена методом из этого класса
	
	void lockedSound_start();
	void lockedSound_end();
	
	bool locked = false;
	
	bool opened = false;
	
	bool doorOpeningOpinion = false;
	
	bool electicaOn = false;
	
	bool door_triggered = false;
	
	bool door2_3_opened = false;
	
	void opening_start();
	void opening_end();
	
	
};



#endif
