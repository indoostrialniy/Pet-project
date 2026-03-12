#pragma once
#ifndef WEAPON_H
#define WEAPON_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

#include "MovingPartsController.h"
#include "global.h"
#include "SoundAsset.h"


class Weapon : public MovingPartsController
{
public:
	Weapon();
	

	bool canBeTaken = true;		// флаг возможности подобрать экземпляр оружия

	bool bSparkSpawned = false;		// флаг спауна сета динамического источника света - вспышки

	//protected:
	float 	maxHitRange = 20.0f;	// эффективная дистанция поражения
	float 	damage = 10.0f;			// наносимый урон
	int 	ammoMaxCap = 1;			// максимальное количество патронов в магазине
	float 	weight = 0.5f;			// вес	
	
	int		currentAmmoCount = ammoMaxCap;

	//for player hands:
	glm::vec4 socketPosition = glm::vec4(0, 0, 0, 1);
	glm::vec4 socketRotationXYZW = glm::vec4(0, 0, 0, 1);
	
	
	//for NPC:
	glm::vec4 socketPositionPlayer = glm::vec4(0, 0, 0, 1);
	glm::vec4 socketRotationPlayerXYZW = glm::vec4(0, 0, 0, 1);
	
	// у любого огнестрельного оружия есть УСМ, курок

	std::string idlePlayerAnimation = "Hands_F";
	std::string aimPlayerAnimation = "Hands_F";
	std::string takingPlayerAnimation = "Hands_F";
	std::string reloadPlayerAnimation = "Hands_F";
	
	
	//~ std::string mainWeaponPart = "default";	// имя главной сущности оружия, на которую будет ориентировано комбо оружия
	

	//public:	
	//~ void logic() override;

	glm::vec4 getSocketPosition();
	glm::vec4 getSocketRotationXYZW();
	
	glm::vec4 getSocketPositionPlayer();
	glm::vec4 getSocketRotationPlayerXYZW();

		
	std::shared_ptr<SoundAsset> weaponShotSoundPtr;
	std::shared_ptr<SoundAsset> weapponNoAmmoSoundPtr;
	std::shared_ptr<SoundAsset> weaponReloadSoundPtr;
	
	
	
	//SoundAssets.getAsset("perezaryadka-orujiya-27703.wav");
	

	
	
	
	// описание всех действий для стрельбы
	//int iWeaponShotTimer = 0;
	bool bWeaponShotReady = true;						// по умолчанию готов к выстрелу
	bool bWeaponShot();							
	void weaponShotTrigger_start();						// колбек начала
	void weaponShotTrigger_end();						// и конца
	void weaponShotShutter_start();
	void weaponShotShutter_end();
	
	
	
	// описание всех действий для перезарядки
	int iWeaponReloadTimer = 0;
	bool bWeaponReload();
	bool bWeaponReloadSequence(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);							
	void weaponReload_start();
	void weaponReload_end();
	
	
	// описание всех действий для прицеливания
	int iWeaponAimTimer = 0;
	bool bWeaponAim( bool status );							
	void bWeaponAim_start();
	void bWeaponAim_end();
	
};





#endif
