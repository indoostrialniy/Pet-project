#pragma once
#ifndef ELECTRIC_BOX_H
#define ELECTRIC_BOX_H

#include "MovingPartsController.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include "SoundAsset.h"
#include "global.h"

class Electric_Box : public MovingPartsController
{
public:	
	Electric_Box( glm::vec3 worldLoc, glm::quat worldRot, std::string n );
	
	bool flag_E = false;	//mainMovingPartsMassiveStatuses[2];		// для обработки клавиши E
	
	bool doorOpen = false;	//mainMovingPartsMassiveStatuses[0];

	sf::Music lightBlinkSound;
	
	sf::Music generatorSound;
	
	
	bool electricBoxActive = false;
	
	std::shared_ptr<SoundAsset> sound_dvercaOpen = SoundAssets.getAsset("safeDoorSound.wav");
	std::shared_ptr<SoundAsset> sound_Fuses = SoundAssets.getAsset("schelchok-vyiklyuchatelya.wav");

	
	//~ static bool turnOnElectroBox(void* ptr, int var);
	
	//~ static bool turnOffElectroBox(void* ptr, int var);
	
	void logic() override;
	
	void open_1_start();

	void open_1_end();

	void open_2_start();

	void open_2_end();

	
};


#endif
