#pragma once
#ifndef TURNIKET_H
#define TURNIKET_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

#include "MovingPartsController.h"
#include "global.h"
#include "SoundAsset.h"

class Turniket_01 : public MovingPartsController
{
public:	
	Turniket_01( glm::vec3 worldLoc, glm::quat worldRot, std::string n);
	
	
	// эти методы будут вешаться как колбеки на секвенсор MovingPartsController и должны удовлетворять требованиям секвенсора - 
	// принимать кастомный тип BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE аргумента и возвращать буль

	//~ bool box_open_script(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	
	//~ bool box_open_script_2(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	
	//~ bool box_close_script(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	
	//~ void test_callback();
	
	//~ bool triggered = false;
	
	//~ void soundOn();
	//~ void soundOff();
	
	//~ std::shared_ptr<SoundAsset> lootingSound 	= SoundAssets.getAsset("looting.wav"); 

	bool front = false;

	void turningSound_start();
	void turningSound_end();
	
	bool triggered = false;
	
	bool lootingHintGiven = false;
	std::shared_ptr<SoundAsset> lootingSound 	= SoundAssets.getAsset("looting.wav"); 

	

	void logic() override; // типовая логика из класса MovingPartsController будет заменена методом из этого класса
};



#endif
