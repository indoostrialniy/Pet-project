#include "SoundAsset.h"

#include <iostream>
#include <nameof.hpp>

#include "macros.h"
#include "auxiliary.h"

//#include <filesystem>
//namespace fs = std::filesystem;


SoundAsset::SoundAsset()
{
	
}

SoundAsset::SoundAsset(std::string resourceName)
{
	#ifdef SOUNDASSET_DEBUG
		std::cout << "Constructor of [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  resourceName << "]" << std::endl;
	#endif	
		
	status = configureSound(resourceName);	
	//DEBUG_INFO
	
	#ifdef SOUNDASSET_DEBUG
		std::cout << "Status of [" << NAMEOF_TYPE(decltype(this)) << "] " <<  status << " " << std::endl;
	#endif		
	//return status;
}



SoundAsset::~SoundAsset()
{
	#ifdef SOUNDASSET_DEBUG
		std::cout << "Destructor of [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  resourceName << "]" << std::endl;
	#endif
}


bool SoundAsset::configureSound(std::string& resourceName)
{
	//std::cout << "\nConfigure Sound: " << resourceName << std::endl;

	std::string temp_path = resourceName;
	for(long long unsigned int i = 0; i< temp_path.length(); i++)
	{
		if( getOperationSystem() == "Windows" )
		{
			if( temp_path[i] == '/' )
			{
				temp_path[i] = '\\';
			}
		}
		else
		{
			if( temp_path[i] == '\\' )
			{
				temp_path[i] = '/';
			}
		}
	}
	//#ifdef SOUNDASSET_DEBUG
		 //std::cout << "Sound correct: " << temp_path << std::endl;
	//#endif
		// std::cout << "Sound full: " << getSoundPath(temp_path) << std::endl;

	//if (!fs::exists(getSoundPath(temp_path)))	{	std::cout << "File does not exist: " << getSoundPath(temp_path) << std::endl;	}
	//else { std::cout << "File exist: " << getSoundPath(temp_path) << std::endl; }

	if ( !soundBuffer.loadFromFile( getSoundPath(temp_path) ) )	// 
	{
		printVarComment("Error loading sound: ", getSoundPath(temp_path) )
		DEBUG_INFO
		return false;
	}
	
	sound = sf::Sound(soundBuffer);
	
	duration = soundBuffer.getDuration().asSeconds();
	
	//sound.play();
	//#ifdef SOUNDASSET_DEBUG
		//std::cout << "Sound done" << std::endl;
	//#endif
	return true;
}

bool SoundAsset::playSound( std::function<void()> start_clb = [](){}, std::function<void()> end_clb = [](){} )
{
	sound.stop();
	sound.play();
	
	BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;		
	std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function = std::bind( &SoundAsset::soundPlaySequence, this, datas);	// главная функция
	//~ auto start = start_clb;		//std::bind( &EXO_Soldier::evac_done_start, this);		// колбек начала
	//~ auto end = end_clb;			//std::bind( &EXO_Soldier::evac_done_end, this);		// колбек конца
		
	Flow.CallSequence( { std::move(function), std::move(start_clb), std::move(end_clb), std::move(datas) } );
	//
	
	return true;
}


// просто пустой цикл, длящийся столько же, сколько звуковая дорожка
bool SoundAsset::soundPlaySequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
{
	if( soundPlayTimer > (int)duration*1000 )
	{
		soundPlayTimer = 0;
		return true;
	}
	soundPlayTimer += tick;
	return false;
}



bool SoundAsset::stopSound()
{
	sound.stop();
	
	return true;
}


std::string SoundAsset::getSoundPath(std::string& filename)
{	
	return getOperationSystem() == "Windows" ? "datas/audio/" + filename + "" : "./datas/audio/" + filename + "";
	//return getOperationSystem() == "Windows" ? "datas\\audio\\" + filename + "" : "./datas/audio/" + filename + "" ;		
}	//тернарный определитель пути




//~ int main()
//~ {
	//~ return 0;
//~ }
