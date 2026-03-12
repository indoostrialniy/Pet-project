#pragma once
#ifndef BOX_01_H
#define BOX_01_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "MovingPartsController.h"

class Box_01 : public MovingPartsController
{
public:	
	Box_01( glm::vec3 worldLoc, glm::quat worldRot, std::string n);
	
	
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

	//~ void lootingSound_start();
	//~ void lootingSound_end();
	

	//~ void logic() override; // типовая логика из класса MovingPartsController будет заменена методом из этого класса
};



#endif
