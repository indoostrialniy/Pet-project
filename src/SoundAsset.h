#pragma once
#ifndef SOUNDASSET_H
#define SOUNDASSET_H

#include <SFML/Audio.hpp>
#include <string>
#include <functional>

#include "global.h"


class SoundAsset
{
public:
	SoundAsset();
	
	SoundAsset(std::string resourceName);
	
	bool status = false;
	
	std::string resourceName = "none";

	
	~SoundAsset();
	
	// Load a sound buffer from a wav file
	sf::SoundBuffer soundBuffer;
	
	sf::Sound 		sound;

	float duration = 0.0f;

	/// zvuk_skripa_bolshih_vorot_kotorie_pitayutsya_otkryt.wav

	// пользовательские переменные и методы
	bool configureSound(std::string& resourceName);
	
	bool playSound( std::function<void()> start_clb, std::function<void()> end_clb );
	bool stopSound();


	int soundPlayTimer = 0;
	bool soundPlaySequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);

	
	std::string getSoundPath(std::string& filename);
	
};



#endif
