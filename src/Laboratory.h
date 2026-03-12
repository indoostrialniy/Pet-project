#pragma once
#ifndef LABORATORY_H
#define LABORATORY_H

#include <SFML/Audio.hpp>

#include "SoundAsset.h"

#include "Level.h"
#include <string>
#include <memory>
#include "global.h"




class Laboratory : public Level
{
public:
	Laboratory();
	
	std::string getSoundPath(std::string filename);

	
	Laboratory(std::string name);
	
	
	bool sound1done = false;
	bool sound2done = false;
	bool sound3done = false;
	bool sound4done = false;
	
	std::shared_ptr<SoundAsset> labScreamer1SoundPtr 	= SoundAssets.getAsset("priblijenie-neizvestnogo.wav"); 
	std::shared_ptr<SoundAsset> lightSourceHint 		= SoundAssets.getAsset("lightSourceHint.wav"); 

	//~ std::shared_ptr<SoundAsset> labScreamer2SoundPtr 	= SoundAssets.getAsset("monstr-ryichit-32517.wav"); 
	
	//~ std::shared_ptr<SoundAsset>labScreamer1SoundPtr 	= SoundAssets.getAsset("monsters\\krik\\prizrak_script_attack_2.ogg"); 
	//~ std::shared_ptr<SoundAsset>labScreamer2SoundPtr 	= SoundAssets.getAsset("monsters\\bloodsucker\\forest_hidden_1.ogg");
	
	
	// Load an ogg music file
    sf::Music labAmbient;
    
    //~ void spawnAllActors() override;

    
	
	void update() override;
	
	~Laboratory();
};


#endif
