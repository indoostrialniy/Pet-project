#include "Pistol.h"

#include "CollisionDescriptor.h"
#include "global.h"

Pistol::Pistol() 
{
	
}

Pistol::Pistol(glm::vec3 worldLoc = glm::vec3(0, 0, 0), glm::quat worldRot = glm::quat(1, 0, 0, 0), std::string name = "default_rifle")
{
	addTag("Weapon");
	addTag("Pistol");

	MicroWorldName = name;

	position = worldLoc;		//назначим координаты блюпринта
	rotation = worldRot;
	
	CollisionDescriptor PistolCollision;
		PistolCollision.collisionRadius = 0.09f;
		PistolCollision.collisionType = SPHERE_COLLISION;
		PistolCollision.complexCollisionFileName = "CB_Pistol_01";

	addEntity( "Magazine", glm::vec3(-0.0741, 0.0, -0.0149), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Magazine", {}, "MESH", { "M_SM_Pistol_01", },  "none",  "forward" );
	addEntity( "Pistol", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Pistol", PistolCollision, "MESH", { "M_SM_Pistol_01", },  "none",  "forward" ); //"CB_Pistol_01"
	addEntity( "Zatvor", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Zatvor", {}, "MESH", { "M_SM_Pistol_01", },  "none",  "forward" );
	addEntity( "Guntrigger", glm::vec3(-0.0162, 0.0, 0.0299), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Guntrigger", {}, "MESH", { "M_SM_Pistol_01", },  "none",  "forward" );

	mainCollisionEntity = "Pistol";	// указываем, какая сущность будет являться основной для комбо оружия
	mainVisualEntity = "Pistol";
	//настраиваем уникальные характеристики конкретного типа оружия
	ammoMaxCap = 7;
	damage = 10;


	currentAmmoCount = ammoMaxCap;

	// укажем вектора ориентации сокета для привязки к руке ГГ
	socketPositionPlayer = glm::vec4(0.5314, -0.121, -0.074, 1);
	socketRotationPlayerXYZW = glm::vec4(-0.39, 0.002, -0.036, 0.920);

	idlePlayerAnimation = "Hands_Pistol_Idle";
	aimPlayerAnimation = "Hands_Pistol_Aim";
	takingPlayerAnimation = "Hands_Pistol_Overview";
	reloadPlayerAnimation = "Hands_Pistol_Reload";

	// обозначим двигающиеся части
	lidProcConfig ZatvorMovingPart;												// структура, кастомизирующая параметры трансформации указанной сущности
		ZatvorMovingPart.name = "Zatvor";
		ZatvorMovingPart.keyCode = GLFW_MOUSE_BUTTON_LEFT;
		ZatvorMovingPart.transformationType = MMP_MOVABLE_REVERSE;
		ZatvorMovingPart.targetPose = glm::vec3(-0.021, 0, 0);
		ZatvorMovingPart.timeForMoving = 70;
	
	lidProcConfig GuntriggerRotatingPart;
		GuntriggerRotatingPart.name = "Guntrigger";
		GuntriggerRotatingPart.mouse = true;
		GuntriggerRotatingPart.keyCode = GLFW_MOUSE_BUTTON_LEFT;
		GuntriggerRotatingPart.transformationType = MMP_ROTATABLE_REVERSE;
		GuntriggerRotatingPart.targetQuat = glm::quat(0.957, 0, 0.291, 0);
		GuntriggerRotatingPart.timeForRotation = 70;
	
	
	mainMovingPartsMassive[MMP_ZERO]   = std::move(ZatvorMovingPart);			// обозначаем двигающуюся часть
	mainMovingPartsMassive[MMP_FIRST]  = std::move(GuntriggerRotatingPart);		// двигающуюся часть

	
	weaponShotSoundPtr 		= SoundAssets.getAsset("weapons/colt/colt_shoot.ogg");
	weapponNoAmmoSoundPtr 	= SoundAssets.getAsset("weapons/colt/colt_empty.ogg");
	weaponReloadSoundPtr 	= SoundAssets.getAsset("weapons/colt/colt_reload.ogg");
	
	
	
	
	// добавим источник света для имитации вспышки выстрела
	//~ if(MicroWorldPersonalLightSourcePtr == nullptr) 
	//~ {
		//~ MicroWorldPersonalLightSourcePtr = new Dynamic_Light( worldLocation, worldRotation, "weaponFire" );  // создадим сет динамического света
		//~ MicroWorldPersonalLightSourcePtr->MicroWorldAttachTo(this, "Pistol", "none", glm::vec4(0.11, 0, 0.06, 1), glm::vec4(0,0,0,1)); //и привяжем его
	//~ }
	
	status = true;	//(components.size() == 4);
}

