#pragma once
#ifndef LEVEL_H
#define LEVEL_H

#include <iostream>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <map>
#include "macros.h"


class MicroWorld;	//~ #include "MicroWorld.h"

struct entityInitializationParameters;



class Level ////базовый класс уровня для описания игрового уровня в целом. Здесь будут списки Сетов и типовая логика уровня. И рендер
{
	
public:
	Level();			// поскольку все уровни будут уточняться через дочерние классы, нужен пустой конструктор базового класса
	
	virtual ~Level();
	
	void levelUpdate();
	
	std::string levelName = "default";

	std::shared_ptr<MicroWorld> getActor(const std::string actorName);
	
	std::shared_ptr<MicroWorld> getTaggedActorNearPoint(const std::string& tagName, const glm::vec3& location, const float& radius);
	
	void destroyActor(const std::string actorName);	// неготовая функция

	
	template <typename T>
	bool spawnActor( std::string comboName, glm::vec3 pos, glm::quat rot)	// если статус создаваемого комбо - false, то он просто не добавится в actors!
	{
		//printVarComment("spawnActor ", comboName)
		if( actors.count(comboName) == 0)
		{
			auto newActor = std::make_shared<T>( pos, rot, comboName );

			if( newActor->getStatus() )
			{
				//printVarComment("Spawn actor OK: ", comboName)
				//DEBUG_INFO
				actors[comboName] = newActor;
				return true;
			}
			else
			{
				printVarComment("Fail to spawn actor. Error configuration ", comboName)
				DEBUG_INFO
				return false;
			}
		}
		else
		{
			printVarComment("Fail to spawn actor. Already exists: ", comboName)
			DEBUG_INFO
			return false;
		}
	}
	
	template <typename T>
	bool spawnActor( std::string comboName )
	{
		//printVarComment("spawnActor ", comboName)
		if( actors.count(comboName) == 0)
		{
			auto newActor = std::make_shared<T>(comboName);
			if( newActor->getStatus() )
			{
				actors[comboName] = newActor;
				return true;
			}
			else
			{
				printVarComment("Fail to spawn actor. Error configuration ", comboName)
				DEBUG_INFO
				return false;
			}
		}
		else
		{
			printVarComment("Fail to spawn actor. Already exists: ", comboName)
			DEBUG_INFO
			return false;
		}
	}
	
	//~ template <typename T>
	//~ bool spawnActor( std::string comboName, glm::vec3 pos, glm::quat rot, std::function<void()> bar);

	//~ int getActorsCount();
	//~ std::shared_ptr<MicroWorld> getActor( int serialNumber );
	

//~ protected:
	std::map <std::string, std::shared_ptr<MicroWorld> > actors; //словарь актеров уровня

public:
	void addComponentToMicroWorld( const std::string& actorName, const entityInitializationParameters& newEntity );

protected:
	
	//~ resManAsset<SoundAsset> sounds("Musics");			// менеджер коротких звуков уровня
	
	std::string getLevelPath(std::string Name);
	
	bool loadLevel(bool log);
	
	virtual void update();
	
	//~ virtual void spawnAllActors();

};




#endif
