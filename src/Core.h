#pragma once
#ifndef CORE_H
#define CORE_H

#include <glm/glm.hpp>
#include <string>

#include "global.h"

class Core
{
public:
	~Core();
	
	void run();
	
	
private:	
	
	glm::fvec2 windowResolution = glm::fvec2(1366.0f,768.0f);
	
	glm::fvec2 windowPosition = glm::fvec2(100,100);

	//bool ambientStarted = false;
	
	bool bFullscreenMode = false;

	std::string configurationFileName = "none";
		
	bool coreLoadConfigs(bool log);

	bool initGL();
	
	void mainLoop();
	
	void cleanUp();
	
	std::string getConfigPath(std::string filename);
	
	// sequences
	bool reminder(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);	//просто функция-напоминалка, но можно сюда повесить некоторые обновления, которые необязательно делать в каждом кадре
	
	bool dynamicLightSourcesUpdater(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	
};


#endif
