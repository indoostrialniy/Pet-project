#pragma once
#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "MovingPartsController.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include "SoundAsset.h"
#include "global.h"

class Elevator : public MovingPartsController
{
public:	
	Elevator( glm::vec3 worldLoc, glm::quat worldRot, std::string n);

	bool flag_E = false;	//mainMovingPartsMassiveStatuses[2];		// для обработки клавиши E
	
	///bool elevatorDoorsOpen = false;//mainMovingPartsMassiveStatuses[0];
	
	//~ std::shared_ptr<SoundAsset> elevatorRadioSoundPtr = SoundAssets.getAsset("mutant_generic_A_3.wav");

	std::shared_ptr<SoundAsset> lampFallingSoundPtr;
	std::shared_ptr<SoundAsset> elevatorOpenIdeaSoundPtr;

	std::shared_ptr<SoundAsset> jumpIdeaSoundPtr = SoundAssets.getAsset("jumpSolution.wav");


	std::shared_ptr<SoundAsset> elevatorOpenDoorSound = SoundAssets.getAsset("elevatorDoorsOpenSound.wav");

	bool bLampFalled = false;
	bool bPlayerInsideCabin = false;
	bool bElevatorTriggered = false;
	
	bool jumpIdeaGiven = false;

	bool focuse_end( BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data );


	int goToGatesTimer = 0;
	
	
	
	static bool goToGates(void* ptr, int var);
	
	static bool openGates(void* ptr, int var);
	
	void logic() override;
	
	
	void lampFalling_start();
	void lampFalling_end();

	void doorApproaching_start();
	void doorApproaching_end();
	
	int elevatorOpenDoorTimer = 0;
	bool doorOpeningSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	void doorOpening_start();
	void doorOpening_end();
	
	bool elevatorDoorsOpened = false;



	
	//~ void update();
	
};




#endif
