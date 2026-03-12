#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <memory>

#include "global.h"

class SoundAsset;	

#include "MicroWorld.h"


class Camera : public MicroWorld // Player
{
public:
	Camera(std::string n );
	~Camera();
	
			
	// параметры камеры
	float fCameraMovingSpeedXY = 5.0f;		// скорость перемещения по плоскости - 5 м/сек
	float fCameraMovingSpeedZ = 4.0f;		// скорость перемещения по вертикали - 2 м/сек
	
	float fCameraRotationSpeed	= 120.0f;	// скорость поворота вокруг оси - 120 град/сек
	
	float fCameraViewDepth = 1000.0f;
	float fCameraFOV = 80.0f; 				// угол обзора камеры
	
	
	bool bPlayerhasWeapon = false;// childMicroWorld == nullptr ? false : true;
	
	int takeWeaponTimer = 0;
	int throwWeaponTimer = 0;

	
	bool flagF = false;
	bool flagH = false;
	
	bool takeE = false;
	bool throwG = false;
	bool flagU = false;
	bool flagR = false;
	bool flagL = false;

	bool flagLMB = false;
	bool flagRMB = false;
	
	void update() override;	// здесь происходит изменение параметров камеры, ОБЯЗАТЕЛЬНО делать поправку на ФПС!
	
	
	std::shared_ptr<SoundAsset> sound_takeWeapon = SoundAssets.getAsset("padenie-orujiya-iz-ryukzaka.wav");
	std::shared_ptr<SoundAsset> sound_walking = SoundAssets.getAsset("korotkiy-chtkiy-bodryiy-zvuk-shaga.wav");
	std::shared_ptr<SoundAsset> sound_LightSwitch = SoundAssets.getAsset("najatie-knopki-fonarika.wav");

	std::shared_ptr<SoundAsset> sound_Pain = SoundAssets.getAsset("ston-mujskoy-bol.wav");

	bool colgunSayed = false;
	std::shared_ptr<SoundAsset> sound_CoolGun = SoundAssets.getAsset("coolGun.wav");


	bool takeWeapon(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	void takeWeapon_start();
	void takeWeapon_end();

	bool throwWeapon(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	void throwWeapon_start();
	void throwWeapon_end();

	//~ static bool throwWeapon(void* ptr, int var);
	
	bool bPlayerAiming = false;
	
	bool pressW = false;
	bool pressS = false;
	bool pressA = false;
	bool pressD = false;
	
	bool lightSourceSpawned = false;
	bool lightSourceActive = false;
	
	void coolGunPhrase_start();
	void coolGunPhrase_end();
	
};




#endif
